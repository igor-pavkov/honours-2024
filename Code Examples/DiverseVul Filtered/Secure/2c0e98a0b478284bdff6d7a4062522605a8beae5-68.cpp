static int mov_write_itunes_hdlr_tag(AVIOContext *pb, MOVMuxContext *mov,
                                     AVFormatContext *s)
{
    avio_wb32(pb, 33); /* size */
    ffio_wfourcc(pb, "hdlr");
    avio_wb32(pb, 0);
    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "mdir");
    ffio_wfourcc(pb, "appl");
    avio_wb32(pb, 0);
    avio_wb32(pb, 0);
    avio_w8(pb, 0);
    return 33;
}