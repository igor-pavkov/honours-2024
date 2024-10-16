static int mov_write_enda_tag(AVIOContext *pb)
{
    avio_wb32(pb, 10);
    ffio_wfourcc(pb, "enda");
    avio_wb16(pb, 1); /* little endian */
    return 10;
}