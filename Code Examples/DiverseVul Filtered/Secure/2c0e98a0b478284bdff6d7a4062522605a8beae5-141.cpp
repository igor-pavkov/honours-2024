static int mov_write_d263_tag(AVIOContext *pb)
{
    avio_wb32(pb, 0xf); /* size */
    ffio_wfourcc(pb, "d263");
    ffio_wfourcc(pb, "FFMP");
    avio_w8(pb, 0); /* decoder version */
    /* FIXME use AVCodecContext level/profile, when encoder will set values */
    avio_w8(pb, 0xa); /* level */
    avio_w8(pb, 0); /* profile */
    return 0xf;
}