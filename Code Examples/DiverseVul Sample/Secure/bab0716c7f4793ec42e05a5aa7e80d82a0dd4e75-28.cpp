static int64_t mxf_set_current_edit_unit(MXFContext *mxf, AVStream *st, int64_t current_offset, int resync)
{
    int64_t next_ofs = -1;
    MXFTrack *track = st->priv_data;
    int64_t edit_unit = av_rescale_q(track->sample_count, st->time_base, av_inv_q(track->edit_rate));
    int64_t new_edit_unit;
    MXFIndexTable *t = mxf_find_index_table(mxf, track->index_sid);

    if (!t || track->wrapping == UnknownWrapped)
        return -1;

    if (mxf_edit_unit_absolute_offset(mxf, t, edit_unit + track->edit_units_per_packet, track->edit_rate, NULL, &next_ofs, NULL, 0) < 0 &&
        (next_ofs = mxf_essence_container_end(mxf, t->body_sid)) <= 0) {
        av_log(mxf->fc, AV_LOG_ERROR, "unable to compute the size of the last packet\n");
        return -1;
    }

    /* check if the next edit unit offset (next_ofs) starts ahead of current_offset */
    if (next_ofs > current_offset)
        return next_ofs;

    if (!resync) {
        av_log(mxf->fc, AV_LOG_ERROR, "cannot find current edit unit for stream %d, invalid index?\n", st->index);
        return -1;
    }

    if (mxf_get_next_track_edit_unit(mxf, track, current_offset + 1, &new_edit_unit) < 0 || new_edit_unit <= 0) {
        av_log(mxf->fc, AV_LOG_ERROR, "failed to find next track edit unit in stream %d\n", st->index);
        return -1;
    }

    new_edit_unit--;
    track->sample_count = mxf_compute_sample_count(mxf, st, new_edit_unit);
    av_log(mxf->fc, AV_LOG_WARNING, "edit unit sync lost on stream %d, jumping from %"PRId64" to %"PRId64"\n", st->index, edit_unit, new_edit_unit);

    return mxf_set_current_edit_unit(mxf, st, current_offset, 0);
}