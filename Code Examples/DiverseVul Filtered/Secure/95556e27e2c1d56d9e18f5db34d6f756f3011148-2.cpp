static int mov_write_gpmd_tag(AVIOContext *pb, const MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "gpmd");
    avio_wb32(pb, 0); /* Reserved */
    avio_wb16(pb, 0); /* Reserved */
    avio_wb16(pb, 1); /* Data-reference index */
    avio_wb32(pb, 0); /* Reserved */
    return update_size(pb, pos);
}