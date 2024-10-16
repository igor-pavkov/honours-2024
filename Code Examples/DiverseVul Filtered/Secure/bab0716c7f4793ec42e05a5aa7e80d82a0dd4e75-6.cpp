static int mxf_get_next_track_edit_unit(MXFContext *mxf, MXFTrack *track, int64_t current_offset, int64_t *edit_unit_out)
{
    int64_t a, b, m, offset;
    MXFIndexTable *t = mxf_find_index_table(mxf, track->index_sid);

    if (!t || track->original_duration <= 0)
        return -1;

    a = -1;
    b = track->original_duration;

    while (b - a > 1) {
        m = (a + b) >> 1;
        if (mxf_edit_unit_absolute_offset(mxf, t, m, track->edit_rate, NULL, &offset, NULL, 0) < 0)
            return -1;
        if (offset < current_offset)
            a = m;
        else
            b = m;
    }

    *edit_unit_out = b;

    return 0;
}