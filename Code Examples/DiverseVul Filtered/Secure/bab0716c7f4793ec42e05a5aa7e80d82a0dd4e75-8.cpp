static int mxf_handle_missing_index_segment(MXFContext *mxf, AVStream *st)
{
    MXFTrack *track = st->priv_data;
    MXFIndexTableSegment *segment = NULL;
    MXFPartition *p = NULL;
    int essence_partition_count = 0;
    int edit_unit_byte_count = 0;
    int i, ret;

    if (!track || track->wrapping != ClipWrapped)
        return 0;

    /* check if track already has an IndexTableSegment */
    for (i = 0; i < mxf->metadata_sets_count; i++) {
        if (mxf->metadata_sets[i]->type == IndexTableSegment) {
            MXFIndexTableSegment *s = (MXFIndexTableSegment*)mxf->metadata_sets[i];
            if (s->body_sid == track->body_sid)
                return 0;
        }
    }

    /* find the essence partition */
    for (i = 0; i < mxf->partitions_count; i++) {
        /* BodySID == 0 -> no essence */
        if (mxf->partitions[i].body_sid != track->body_sid)
            continue;

        p = &mxf->partitions[i];
        essence_partition_count++;
    }

    /* only handle files with a single essence partition */
    if (essence_partition_count != 1)
        return 0;

    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && is_pcm(st->codecpar->codec_id)) {
        edit_unit_byte_count = (av_get_bits_per_sample(st->codecpar->codec_id) * st->codecpar->channels) >> 3;
    } else if (st->duration > 0 && p->first_essence_klv.length > 0 && p->first_essence_klv.length % st->duration == 0) {
        edit_unit_byte_count = p->first_essence_klv.length / st->duration;
    }

    if (edit_unit_byte_count <= 0)
        return 0;

    av_log(mxf->fc, AV_LOG_WARNING, "guessing index for stream %d using edit unit byte count %d\n", st->index, edit_unit_byte_count);

    if (!(segment = av_mallocz(sizeof(*segment))))
        return AVERROR(ENOMEM);

    if ((ret = mxf_add_metadata_set(mxf, segment))) {
        mxf_free_metadataset((MXFMetadataSet**)&segment, 1);
        return ret;
    }

    /* Make sure we have nonzero unique index_sid, body_sid will be ok, because
     * using the same SID for index is forbidden in MXF. */
    if (!track->index_sid)
        track->index_sid = track->body_sid;

    segment->type = IndexTableSegment;
    /* stream will be treated as small EditUnitByteCount */
    segment->edit_unit_byte_count = edit_unit_byte_count;
    segment->index_start_position = 0;
    segment->index_duration = st->duration;
    segment->index_edit_rate = av_inv_q(st->time_base);
    segment->index_sid = track->index_sid;
    segment->body_sid = p->body_sid;
    return 0;
}