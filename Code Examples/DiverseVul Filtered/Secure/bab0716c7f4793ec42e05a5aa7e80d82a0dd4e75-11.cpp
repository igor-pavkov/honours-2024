static int mxf_absolute_bodysid_offset(MXFContext *mxf, int body_sid, int64_t offset, int64_t *offset_out, MXFPartition **partition_out)
{
    MXFPartition *last_p = NULL;
    int a, b, m, m0;

    if (offset < 0)
        return AVERROR(EINVAL);

    a = -1;
    b = mxf->partitions_count;

    while (b - a > 1) {
        m0 = m = (a + b) >> 1;

        while (m < b && mxf->partitions[m].body_sid != body_sid)
            m++;

        if (m < b && mxf->partitions[m].body_offset <= offset)
            a = m;
        else
            b = m0;
    }

    if (a >= 0)
        last_p = &mxf->partitions[a];

    if (last_p && (!last_p->essence_length || last_p->essence_length > (offset - last_p->body_offset))) {
        *offset_out = last_p->essence_offset + (offset - last_p->body_offset);
        if (partition_out)
            *partition_out = last_p;
        return 0;
    }

    av_log(mxf->fc, AV_LOG_ERROR,
           "failed to find absolute offset of %"PRIX64" in BodySID %i - partial file?\n",
           offset, body_sid);

    return AVERROR_INVALIDDATA;
}