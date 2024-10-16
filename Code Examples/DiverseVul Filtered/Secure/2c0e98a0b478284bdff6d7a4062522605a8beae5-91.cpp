static void mov_write_psp_udta_tag(AVIOContext *pb,
                                   const char *str, const char *lang, int type)
{
    int len = utf8len(str) + 1;
    if (len <= 0)
        return;
    avio_wb16(pb, len * 2 + 10);        /* size */
    avio_wb32(pb, type);                /* type */
    avio_wb16(pb, language_code(lang)); /* language */
    avio_wb16(pb, 0x01);                /* ? */
    ascii_to_wc(pb, str);
}