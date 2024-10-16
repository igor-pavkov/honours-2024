static int mov_write_clap_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 40);
    ffio_wfourcc(pb, "clap");
    avio_wb32(pb, track->par->width); /* apertureWidth_N */
    avio_wb32(pb, 1); /* apertureWidth_D (= 1) */
    avio_wb32(pb, track->height); /* apertureHeight_N */
    avio_wb32(pb, 1); /* apertureHeight_D (= 1) */
    avio_wb32(pb, 0); /* horizOff_N (= 0) */
    avio_wb32(pb, 1); /* horizOff_D (= 1) */
    avio_wb32(pb, 0); /* vertOff_N (= 0) */
    avio_wb32(pb, 1); /* vertOff_D (= 1) */
    return 40;
}