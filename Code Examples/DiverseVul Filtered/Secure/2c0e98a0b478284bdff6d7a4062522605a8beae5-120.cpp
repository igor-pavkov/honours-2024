static int mov_write_tmpo_tag(AVIOContext *pb, AVFormatContext *s)
{
    AVDictionaryEntry *t = av_dict_get(s->metadata, "tmpo", NULL, 0);
    int size = 0, tmpo = t ? atoi(t->value) : 0;
    if (tmpo) {
        size = 26;
        avio_wb32(pb, size);
        ffio_wfourcc(pb, "tmpo");
        avio_wb32(pb, size-8); /* size */
        ffio_wfourcc(pb, "data");
        avio_wb32(pb, 0x15);  //type specifier
        avio_wb32(pb, 0);
        avio_wb16(pb, tmpo);        // data
    }
    return size;
}