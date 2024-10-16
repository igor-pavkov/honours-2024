static int mov_write_dpxe_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 12);
    ffio_wfourcc(pb, "DpxE");
    if (track->par->extradata_size >= 12 &&
        !memcmp(&track->par->extradata[4], "DpxE", 4)) {
        avio_wb32(pb, track->par->extradata[11]);
    } else {
        avio_wb32(pb, 1);
    }
    return 0;
}