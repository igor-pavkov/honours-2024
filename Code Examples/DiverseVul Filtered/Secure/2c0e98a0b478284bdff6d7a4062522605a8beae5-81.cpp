static int mov_write_trex_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 0x20); /* size */
    ffio_wfourcc(pb, "trex");
    avio_wb32(pb, 0);   /* version & flags */
    avio_wb32(pb, track->track_id); /* track ID */
    avio_wb32(pb, 1);   /* default sample description index */
    avio_wb32(pb, 0);   /* default sample duration */
    avio_wb32(pb, 0);   /* default sample size */
    avio_wb32(pb, 0);   /* default sample flags */
    return 0;
}