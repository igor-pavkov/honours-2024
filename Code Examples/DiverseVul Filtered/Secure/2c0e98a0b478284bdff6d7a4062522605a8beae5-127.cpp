static int mov_write_rtp_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "rtp ");
    avio_wb32(pb, 0); /* Reserved */
    avio_wb16(pb, 0); /* Reserved */
    avio_wb16(pb, 1); /* Data-reference index */

    avio_wb16(pb, 1); /* Hint track version */
    avio_wb16(pb, 1); /* Highest compatible version */
    avio_wb32(pb, track->max_packet_size); /* Max packet size */

    avio_wb32(pb, 12); /* size */
    ffio_wfourcc(pb, "tims");
    avio_wb32(pb, track->timescale);

    return update_size(pb, pos);
}