static int mxf_read_seek(AVFormatContext *s, int stream_index, int64_t sample_time, int flags)
{
    AVStream *st = s->streams[stream_index];
    int64_t seconds;
    MXFContext* mxf = s->priv_data;
    int64_t seekpos;
    int i, ret;
    MXFIndexTable *t;
    MXFTrack *source_track = st->priv_data;

    if (!source_track)
        return 0;

    /* if audio then truncate sample_time to EditRate */
    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        sample_time = av_rescale_q(sample_time, st->time_base,
                                   av_inv_q(source_track->edit_rate));

    if (mxf->nb_index_tables <= 0) {
        if (!s->bit_rate)
            return AVERROR_INVALIDDATA;
        if (sample_time < 0)
            sample_time = 0;
        seconds = av_rescale(sample_time, st->time_base.num, st->time_base.den);

        seekpos = avio_seek(s->pb, (s->bit_rate * seconds) >> 3, SEEK_SET);
        if (seekpos < 0)
            return seekpos;

        ff_update_cur_dts(s, st, sample_time);
        mxf->current_klv_data = (KLVPacket){{0}};
    } else {
        MXFPartition *partition;

        t = &mxf->index_tables[0];
        if (t->index_sid != source_track->index_sid) {
            /* If the first index table does not belong to the stream, then find a stream which does belong to the index table */
            for (i = 0; i < s->nb_streams; i++) {
                MXFTrack *new_source_track = s->streams[i]->priv_data;
                if (new_source_track && new_source_track->index_sid == t->index_sid) {
                    sample_time = av_rescale_q(sample_time, new_source_track->edit_rate, source_track->edit_rate);
                    source_track = new_source_track;
                    st = s->streams[i];
                    break;
                }
            }
            if (i == s->nb_streams)
                return AVERROR_INVALIDDATA;
        }

        /* clamp above zero, else ff_index_search_timestamp() returns negative
         * this also means we allow seeking before the start */
        sample_time = FFMAX(sample_time, 0);

        if (t->fake_index) {
            /* The first frames may not be keyframes in presentation order, so
             * we have to advance the target to be able to find the first
             * keyframe backwards... */
            if (!(flags & AVSEEK_FLAG_ANY) &&
                (flags & AVSEEK_FLAG_BACKWARD) &&
                t->ptses[0] != AV_NOPTS_VALUE &&
                sample_time < t->ptses[0] &&
                (t->fake_index[t->ptses[0]].flags & AVINDEX_KEYFRAME))
                sample_time = t->ptses[0];

            /* behave as if we have a proper index */
            if ((sample_time = ff_index_search_timestamp(t->fake_index, t->nb_ptses, sample_time, flags)) < 0)
                return sample_time;
            /* get the stored order index from the display order index */
            sample_time += t->offsets[sample_time];
        } else {
            /* no IndexEntryArray (one or more CBR segments)
             * make sure we don't seek past the end */
            sample_time = FFMIN(sample_time, source_track->original_duration - 1);
        }

        if (source_track->wrapping == UnknownWrapped)
            av_log(mxf->fc, AV_LOG_WARNING, "attempted seek in an UnknownWrapped essence\n");

        if ((ret = mxf_edit_unit_absolute_offset(mxf, t, sample_time, source_track->edit_rate, &sample_time, &seekpos, &partition, 1)) < 0)
            return ret;

        ff_update_cur_dts(s, st, sample_time);
        if (source_track->wrapping == ClipWrapped) {
            KLVPacket klv = partition->first_essence_klv;
            if (seekpos < klv.next_klv - klv.length || seekpos >= klv.next_klv) {
                av_log(mxf->fc, AV_LOG_ERROR, "attempted seek out of clip wrapped KLV\n");
                return AVERROR_INVALIDDATA;
            }
            mxf->current_klv_data = klv;
        } else {
            mxf->current_klv_data = (KLVPacket){{0}};
        }
        avio_seek(s->pb, seekpos, SEEK_SET);
    }

    // Update all tracks sample count
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *cur_st = s->streams[i];
        MXFTrack *cur_track = cur_st->priv_data;
        if (cur_track) {
            int64_t track_edit_unit = sample_time;
            if (st != cur_st)
                mxf_get_next_track_edit_unit(mxf, cur_track, seekpos, &track_edit_unit);
            cur_track->sample_count = mxf_compute_sample_count(mxf, cur_st, track_edit_unit);
        }
    }
    return 0;
}