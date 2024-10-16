static int mov_write_uuidusmt_tag(AVIOContext *pb, AVFormatContext *s)
{
    AVDictionaryEntry *title = av_dict_get(s->metadata, "title", NULL, 0);
    int64_t pos, pos2;

    if (title) {
        pos = avio_tell(pb);
        avio_wb32(pb, 0); /* size placeholder*/
        ffio_wfourcc(pb, "uuid");
        ffio_wfourcc(pb, "USMT");
        avio_wb32(pb, 0x21d24fce); /* 96 bit UUID */
        avio_wb32(pb, 0xbb88695c);
        avio_wb32(pb, 0xfac9c740);

        pos2 = avio_tell(pb);
        avio_wb32(pb, 0); /* size placeholder*/
        ffio_wfourcc(pb, "MTDT");
        avio_wb16(pb, 4);

        // ?
        avio_wb16(pb, 0x0C);                 /* size */
        avio_wb32(pb, 0x0B);                 /* type */
        avio_wb16(pb, language_code("und")); /* language */
        avio_wb16(pb, 0x0);                  /* ? */
        avio_wb16(pb, 0x021C);               /* data */

        if (!(s->flags & AVFMT_FLAG_BITEXACT))
            mov_write_psp_udta_tag(pb, LIBAVCODEC_IDENT,      "eng", 0x04);
        mov_write_psp_udta_tag(pb, title->value,          "eng", 0x01);
        mov_write_psp_udta_tag(pb, "2006/04/01 11:11:11", "und", 0x03);

        update_size(pb, pos2);
        return update_size(pb, pos);
    }

    return 0;
}