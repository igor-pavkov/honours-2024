static int mov_write_dref_tag(AVIOContext *pb)
{
    avio_wb32(pb, 28); /* size */
    ffio_wfourcc(pb, "dref");
    avio_wb32(pb, 0); /* version & flags */
    avio_wb32(pb, 1); /* entry count */

    avio_wb32(pb, 0xc); /* size */
    //FIXME add the alis and rsrc atom
    ffio_wfourcc(pb, "url ");
    avio_wb32(pb, 1); /* version & flags */

    return 28;
}