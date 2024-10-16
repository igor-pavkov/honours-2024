static int mov_write_chpl_tag(AVIOContext *pb, AVFormatContext *s)
{
    int64_t pos = avio_tell(pb);
    int i, nb_chapters = FFMIN(s->nb_chapters, 255);

    avio_wb32(pb, 0);            // size
    ffio_wfourcc(pb, "chpl");
    avio_wb32(pb, 0x01000000);   // version + flags
    avio_wb32(pb, 0);            // unknown
    avio_w8(pb, nb_chapters);

    for (i = 0; i < nb_chapters; i++) {
        AVChapter *c = s->chapters[i];
        AVDictionaryEntry *t;
        avio_wb64(pb, av_rescale_q(c->start, c->time_base, (AVRational){1,10000000}));

        if ((t = av_dict_get(c->metadata, "title", NULL, 0))) {
            int len = FFMIN(strlen(t->value), 255);
            avio_w8(pb, len);
            avio_write(pb, t->value, len);
        } else
            avio_w8(pb, 0);
    }
    return update_size(pb, pos);
}