static int mov_write_dinf_tag(AVIOContext *pb)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "dinf");
    mov_write_dref_tag(pb);
    return update_size(pb, pos);
}