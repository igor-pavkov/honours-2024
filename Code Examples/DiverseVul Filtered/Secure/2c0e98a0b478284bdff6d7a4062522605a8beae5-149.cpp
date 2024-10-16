static int mov_write_3gp_udta_tag(AVIOContext *pb, AVFormatContext *s,
                                  const char *tag, const char *str)
{
    int64_t pos = avio_tell(pb);
    AVDictionaryEntry *t = av_dict_get(s->metadata, str, NULL, 0);
    if (!t || !utf8len(t->value))
        return 0;
    avio_wb32(pb, 0);   /* size */
    ffio_wfourcc(pb, tag); /* type */
    avio_wb32(pb, 0);   /* version + flags */
    if (!strcmp(tag, "yrrc"))
        avio_wb16(pb, atoi(t->value));
    else {
        avio_wb16(pb, language_code("eng")); /* language */
        avio_write(pb, t->value, strlen(t->value) + 1); /* UTF8 string value */
        if (!strcmp(tag, "albm") &&
            (t = av_dict_get(s->metadata, "track", NULL, 0)))
            avio_w8(pb, atoi(t->value));
    }
    return update_size(pb, pos);
}