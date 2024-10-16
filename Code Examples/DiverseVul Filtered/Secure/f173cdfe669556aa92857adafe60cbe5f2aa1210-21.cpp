static int mxf_parse_handle_essence(MXFContext *mxf)
{
    AVIOContext *pb = mxf->fc->pb;
    int64_t ret;

    if (mxf->parsing_backward) {
        return mxf_seek_to_previous_partition(mxf);
    } else {
        uint64_t offset = mxf->footer_partition ? mxf->footer_partition
                                                : mxf->last_partition;

        if (!offset) {
            av_dlog(mxf->fc, "no last partition\n");
            return 0;
        }

        av_dlog(mxf->fc, "seeking to last partition\n");

        /* remember where we were so we don't end up seeking further back than this */
        mxf->last_forward_tell = avio_tell(pb);

        if (!pb->seekable) {
            av_log(mxf->fc, AV_LOG_INFO, "file is not seekable - not parsing last partition\n");
            return -1;
        }

        /* seek to last partition and parse backward */
        if ((ret = avio_seek(pb, mxf->run_in + offset, SEEK_SET)) < 0) {
            av_log(mxf->fc, AV_LOG_ERROR,
                   "failed to seek to last partition @ 0x%" PRIx64
                   " (%"PRId64") - partial file?\n",
                   mxf->run_in + offset, ret);
            return ret;
        }

        mxf->current_partition = NULL;
        mxf->parsing_backward = 1;
    }

    return 1;
}