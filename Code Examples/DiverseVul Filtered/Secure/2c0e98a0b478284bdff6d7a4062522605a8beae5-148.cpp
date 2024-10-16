static int mov_write_mdta_keys_tag(AVIOContext *pb, MOVMuxContext *mov,
                                   AVFormatContext *s)
{
    AVDictionaryEntry *t = NULL;
    int64_t pos = avio_tell(pb);
    int64_t curpos, entry_pos;
    int count = 0;

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "keys");
    avio_wb32(pb, 0);
    entry_pos = avio_tell(pb);
    avio_wb32(pb, 0); /* entry count */

    while (t = av_dict_get(s->metadata, "", t, AV_DICT_IGNORE_SUFFIX)) {
        avio_wb32(pb, strlen(t->key) + 8);
        ffio_wfourcc(pb, "mdta");
        avio_write(pb, t->key, strlen(t->key));
        count += 1;
    }
    curpos = avio_tell(pb);
    avio_seek(pb, entry_pos, SEEK_SET);
    avio_wb32(pb, count); // rewrite entry count
    avio_seek(pb, curpos, SEEK_SET);

    return update_size(pb, pos);
}