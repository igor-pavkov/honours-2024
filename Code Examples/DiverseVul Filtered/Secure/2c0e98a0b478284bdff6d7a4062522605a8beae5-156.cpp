static int mov_flush_fragment(AVFormatContext *s, int force)
{
    MOVMuxContext *mov = s->priv_data;
    int i, first_track = -1;
    int64_t mdat_size = 0;
    int ret;
    int has_video = 0, starts_with_key = 0, first_video_track = 1;

    if (!(mov->flags & FF_MOV_FLAG_FRAGMENT))
        return 0;

    // Try to fill in the duration of the last packet in each stream
    // from queued packets in the interleave queues. If the flushing
    // of fragments was triggered automatically by an AVPacket, we
    // already have reliable info for the end of that track, but other
    // tracks may need to be filled in.
    for (i = 0; i < s->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if (!track->end_reliable) {
            int64_t ts_offset;
            const AVPacket *next = ff_interleaved_peek(s, i, &ts_offset);
            if (next) {
                track->track_duration = next->dts - track->start_dts + ts_offset;
                if (next->pts != AV_NOPTS_VALUE)
                    track->end_pts = next->pts;
                else
                    track->end_pts = next->dts;
                track->end_pts += ts_offset;
            }
        }
    }

    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if (track->entry <= 1)
            continue;
        // Sample durations are calculated as the diff of dts values,
        // but for the last sample in a fragment, we don't know the dts
        // of the first sample in the next fragment, so we have to rely
        // on what was set as duration in the AVPacket. Not all callers
        // set this though, so we might want to replace it with an
        // estimate if it currently is zero.
        if (get_cluster_duration(track, track->entry - 1) != 0)
            continue;
        // Use the duration (i.e. dts diff) of the second last sample for
        // the last one. This is a wild guess (and fatal if it turns out
        // to be too long), but probably the best we can do - having a zero
        // duration is bad as well.
        track->track_duration += get_cluster_duration(track, track->entry - 2);
        track->end_pts        += get_cluster_duration(track, track->entry - 2);
        if (!mov->missing_duration_warned) {
            av_log(s, AV_LOG_WARNING,
                   "Estimating the duration of the last packet in a "
                   "fragment, consider setting the duration field in "
                   "AVPacket instead.\n");
            mov->missing_duration_warned = 1;
        }
    }

    if (!mov->moov_written) {
        int64_t pos = avio_tell(s->pb);
        uint8_t *buf;
        int buf_size, moov_size;

        for (i = 0; i < mov->nb_streams; i++)
            if (!mov->tracks[i].entry)
                break;
        /* Don't write the initial moov unless all tracks have data */
        if (i < mov->nb_streams && !force)
            return 0;

        moov_size = get_moov_size(s);
        for (i = 0; i < mov->nb_streams; i++)
            mov->tracks[i].data_offset = pos + moov_size + 8;

        avio_write_marker(s->pb, AV_NOPTS_VALUE, AVIO_DATA_MARKER_HEADER);
        if (mov->flags & FF_MOV_FLAG_DELAY_MOOV)
            mov_write_identification(s->pb, s);
        if ((ret = mov_write_moov_tag(s->pb, mov, s)) < 0)
            return ret;

        if (mov->flags & FF_MOV_FLAG_DELAY_MOOV) {
            if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX)
                mov->reserved_header_pos = avio_tell(s->pb);
            avio_flush(s->pb);
            mov->moov_written = 1;
            return 0;
        }

        buf_size = avio_close_dyn_buf(mov->mdat_buf, &buf);
        mov->mdat_buf = NULL;
        avio_wb32(s->pb, buf_size + 8);
        ffio_wfourcc(s->pb, "mdat");
        avio_write(s->pb, buf, buf_size);
        av_free(buf);

        if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX)
            mov->reserved_header_pos = avio_tell(s->pb);

        mov->moov_written = 1;
        mov->mdat_size = 0;
        for (i = 0; i < mov->nb_streams; i++) {
            if (mov->tracks[i].entry)
                mov->tracks[i].frag_start += mov->tracks[i].start_dts +
                                             mov->tracks[i].track_duration -
                                             mov->tracks[i].cluster[0].dts;
            mov->tracks[i].entry = 0;
            mov->tracks[i].end_reliable = 0;
        }
        avio_flush(s->pb);
        return 0;
    }

    if (mov->frag_interleave) {
        for (i = 0; i < mov->nb_streams; i++) {
            MOVTrack *track = &mov->tracks[i];
            int ret;
            if ((ret = mov_flush_fragment_interleaving(s, track)) < 0)
                return ret;
        }

        if (!mov->mdat_buf)
            return 0;
        mdat_size = avio_tell(mov->mdat_buf);
    }

    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if (mov->flags & FF_MOV_FLAG_SEPARATE_MOOF || mov->frag_interleave)
            track->data_offset = 0;
        else
            track->data_offset = mdat_size;
        if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            has_video = 1;
            if (first_video_track) {
                if (track->entry)
                    starts_with_key = track->cluster[0].flags & MOV_SYNC_SAMPLE;
                first_video_track = 0;
            }
        }
        if (!track->entry)
            continue;
        if (track->mdat_buf)
            mdat_size += avio_tell(track->mdat_buf);
        if (first_track < 0)
            first_track = i;
    }

    if (!mdat_size)
        return 0;

    avio_write_marker(s->pb,
                      av_rescale(mov->tracks[first_track].cluster[0].dts, AV_TIME_BASE, mov->tracks[first_track].timescale),
                      (has_video ? starts_with_key : mov->tracks[first_track].cluster[0].flags & MOV_SYNC_SAMPLE) ? AVIO_DATA_MARKER_SYNC_POINT : AVIO_DATA_MARKER_BOUNDARY_POINT);

    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        int buf_size, write_moof = 1, moof_tracks = -1;
        uint8_t *buf;
        int64_t duration = 0;

        if (track->entry)
            duration = track->start_dts + track->track_duration -
                       track->cluster[0].dts;
        if (mov->flags & FF_MOV_FLAG_SEPARATE_MOOF) {
            if (!track->mdat_buf)
                continue;
            mdat_size = avio_tell(track->mdat_buf);
            moof_tracks = i;
        } else {
            write_moof = i == first_track;
        }

        if (write_moof) {
            avio_flush(s->pb);

            mov_write_moof_tag(s->pb, mov, moof_tracks, mdat_size);
            mov->fragments++;

            avio_wb32(s->pb, mdat_size + 8);
            ffio_wfourcc(s->pb, "mdat");
        }

        if (track->entry)
            track->frag_start += duration;
        track->entry = 0;
        track->entries_flushed = 0;
        track->end_reliable = 0;
        if (!mov->frag_interleave) {
            if (!track->mdat_buf)
                continue;
            buf_size = avio_close_dyn_buf(track->mdat_buf, &buf);
            track->mdat_buf = NULL;
        } else {
            if (!mov->mdat_buf)
                continue;
            buf_size = avio_close_dyn_buf(mov->mdat_buf, &buf);
            mov->mdat_buf = NULL;
        }

        avio_write(s->pb, buf, buf_size);
        av_free(buf);
    }

    mov->mdat_size = 0;

    avio_flush(s->pb);
    return 0;
}