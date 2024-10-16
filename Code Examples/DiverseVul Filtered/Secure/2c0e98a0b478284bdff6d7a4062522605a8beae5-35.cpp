static int mov_write_source_reference_tag(AVIOContext *pb, MOVTrack *track, const char *reel_name)
{
    uint64_t str_size =strlen(reel_name);
    int64_t pos = avio_tell(pb);

    if (str_size >= UINT16_MAX){
        av_log(NULL, AV_LOG_ERROR, "reel_name length %"PRIu64" is too large\n", str_size);
        avio_wb16(pb, 0);
        return AVERROR(EINVAL);
    }

    avio_wb32(pb, 0);                              /* size */
    ffio_wfourcc(pb, "name");                      /* Data format */
    avio_wb16(pb, str_size);                       /* string size */
    avio_wb16(pb, track->language);                /* langcode */
    avio_write(pb, reel_name, str_size);           /* reel name */
    return update_size(pb,pos);
}