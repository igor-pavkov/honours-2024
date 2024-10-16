static int mov_write_mfhd_tag(AVIOContext *pb, MOVMuxContext *mov)
{
    avio_wb32(pb, 16);
    ffio_wfourcc(pb, "mfhd");
    avio_wb32(pb, 0);
    avio_wb32(pb, mov->fragments);
    return 0;
}