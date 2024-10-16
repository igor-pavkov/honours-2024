static int mov_write_uuid_tag_psp(AVIOContext *pb, MOVTrack *mov)
{
    avio_wb32(pb, 0x34); /* size ... reports as 28 in mp4box! */
    ffio_wfourcc(pb, "uuid");
    ffio_wfourcc(pb, "USMT");
    avio_wb32(pb, 0x21d24fce);
    avio_wb32(pb, 0xbb88695c);
    avio_wb32(pb, 0xfac9c740);
    avio_wb32(pb, 0x1c);     // another size here!
    ffio_wfourcc(pb, "MTDT");
    avio_wb32(pb, 0x00010012);
    avio_wb32(pb, 0x0a);
    avio_wb32(pb, 0x55c40000);
    avio_wb32(pb, 0x1);
    avio_wb32(pb, 0x0);
    return 0x34;
}