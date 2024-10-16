static int mov_write_raw_metadata_tag(AVFormatContext *s, AVIOContext *pb,
                                      const char *name, const char *key)
{
    int len;
    AVDictionaryEntry *t;

    if (!(t = av_dict_get(s->metadata, key, NULL, 0)))
        return 0;

    len = strlen(t->value);
    if (len > 0) {
        int size = len + 8;
        avio_wb32(pb, size);
        ffio_wfourcc(pb, name);
        avio_write(pb, t->value, len);
        return size;
    }
    return 0;
}