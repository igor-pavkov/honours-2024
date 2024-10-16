static int mov_write_meta_tag(AVIOContext *pb, MOVMuxContext *mov,
                              AVFormatContext *s)
{
    int size = 0;
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "meta");
    avio_wb32(pb, 0);
    if (mov->flags & FF_MOV_FLAG_USE_MDTA) {
        mov_write_mdta_hdlr_tag(pb, mov, s);
        mov_write_mdta_keys_tag(pb, mov, s);
        mov_write_mdta_ilst_tag(pb, mov, s);
    }
    else {
        /* iTunes metadata tag */
        mov_write_itunes_hdlr_tag(pb, mov, s);
        mov_write_ilst_tag(pb, mov, s);
    }
    size = update_size(pb, pos);
    return size;
}