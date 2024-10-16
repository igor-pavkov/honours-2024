static int mov_write_string_metadata(AVFormatContext *s, AVIOContext *pb,
                                     const char *name, const char *tag,
                                     int long_style)
{
    int lang;
    AVDictionaryEntry *t = get_metadata_lang(s, tag, &lang);
    if (!t)
        return 0;
    return mov_write_string_tag(pb, name, t->value, lang, long_style);
}