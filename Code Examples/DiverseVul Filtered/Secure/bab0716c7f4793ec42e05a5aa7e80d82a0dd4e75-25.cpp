static int mxf_read_content_storage(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFContext *mxf = arg;
    switch (tag) {
    case 0x1901:
        if (mxf->packages_refs)
            av_log(mxf->fc, AV_LOG_VERBOSE, "Multiple packages_refs\n");
        av_free(mxf->packages_refs);
        return mxf_read_strong_ref_array(pb, &mxf->packages_refs, &mxf->packages_count);
    case 0x1902:
        av_free(mxf->essence_container_data_refs);
        return mxf_read_strong_ref_array(pb, &mxf->essence_container_data_refs, &mxf->essence_container_data_count);
    }
    return 0;
}