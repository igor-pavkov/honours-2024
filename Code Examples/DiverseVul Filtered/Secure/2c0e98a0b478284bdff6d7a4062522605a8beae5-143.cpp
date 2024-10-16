static int mov_write_gmhd_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);      /* size */
    ffio_wfourcc(pb, "gmhd");
    avio_wb32(pb, 0x18);   /* gmin size */
    ffio_wfourcc(pb, "gmin");/* generic media info */
    avio_wb32(pb, 0);      /* version & flags */
    avio_wb16(pb, 0x40);   /* graphics mode = */
    avio_wb16(pb, 0x8000); /* opColor (r?) */
    avio_wb16(pb, 0x8000); /* opColor (g?) */
    avio_wb16(pb, 0x8000); /* opColor (b?) */
    avio_wb16(pb, 0);      /* balance */
    avio_wb16(pb, 0);      /* reserved */

    /*
     * This special text atom is required for
     * Apple Quicktime chapters. The contents
     * don't appear to be documented, so the
     * bytes are copied verbatim.
     */
    if (track->tag != MKTAG('c','6','0','8')) {
    avio_wb32(pb, 0x2C);   /* size */
    ffio_wfourcc(pb, "text");
    avio_wb16(pb, 0x01);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x01);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00004000);
    avio_wb16(pb, 0x0000);
    }

    if (track->par->codec_tag == MKTAG('t','m','c','d')) {
        int64_t tmcd_pos = avio_tell(pb);
        avio_wb32(pb, 0); /* size */
        ffio_wfourcc(pb, "tmcd");
        mov_write_tcmi_tag(pb, track);
        update_size(pb, tmcd_pos);
    }
    return update_size(pb, pos);
}