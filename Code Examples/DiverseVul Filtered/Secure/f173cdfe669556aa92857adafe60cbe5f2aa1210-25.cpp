static int mxf_seek_to_previous_partition(MXFContext *mxf)
{
    AVIOContext *pb = mxf->fc->pb;

    if (!mxf->current_partition ||
        mxf->run_in + mxf->current_partition->previous_partition <= mxf->last_forward_tell)
        return 0;   /* we've parsed all partitions */

    /* seek to previous partition */
    avio_seek(pb, mxf->run_in + mxf->current_partition->previous_partition, SEEK_SET);
    mxf->current_partition = NULL;

    av_dlog(mxf->fc, "seeking to previous partition\n");

    return 1;
}