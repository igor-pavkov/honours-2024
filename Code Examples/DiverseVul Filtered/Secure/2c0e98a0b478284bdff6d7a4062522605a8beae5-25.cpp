static int mov_write_hmhd_tag(AVIOContext *pb)
{
    /* This atom must be present, but leaving the values at zero
     * seems harmless. */
    avio_wb32(pb, 28); /* size */
    ffio_wfourcc(pb, "hmhd");
    avio_wb32(pb, 0); /* version, flags */
    avio_wb16(pb, 0); /* maxPDUsize */
    avio_wb16(pb, 0); /* avgPDUsize */
    avio_wb32(pb, 0); /* maxbitrate */
    avio_wb32(pb, 0); /* avgbitrate */
    avio_wb32(pb, 0); /* reserved */
    return 28;
}