static int find_body_sid_by_offset(MXFContext *mxf, int64_t offset)
{
    // we look for partition where the offset is placed
    int a, b, m;
    int64_t this_partition;

    a = -1;
    b = mxf->partitions_count;

    while (b - a > 1) {
        m         = (a + b) >> 1;
        this_partition = mxf->partitions[m].this_partition;
        if (this_partition <= offset)
            a = m;
        else
            b = m;
    }

    if (a == -1)
        return 0;
    return mxf->partitions[a].body_sid;
}