static int mov_write_nmhd_tag(AVIOContext *pb)
{
    avio_wb32(pb, 12);
    ffio_wfourcc(pb, "nmhd");
    avio_wb32(pb, 0);
    return 12;
}