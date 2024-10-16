static int mov_write_int8_metadata(AVFormatContext *s, AVIOContext *pb,
                                   const char *name, const char *tag,
                                   int len)
{
    AVDictionaryEntry *t = NULL;
    uint8_t num;
    int size = 24 + len;

    if (len != 1 && len != 4)
        return -1;

    if (!(t = av_dict_get(s->metadata, tag, NULL, 0)))
        return 0;
    num = atoi(t->value);

    avio_wb32(pb, size);
    ffio_wfourcc(pb, name);
    avio_wb32(pb, size - 8);
    ffio_wfourcc(pb, "data");
    avio_wb32(pb, 0x15);
    avio_wb32(pb, 0);
    if (len==4) avio_wb32(pb, num);
    else        avio_w8 (pb, num);

    return size;
}