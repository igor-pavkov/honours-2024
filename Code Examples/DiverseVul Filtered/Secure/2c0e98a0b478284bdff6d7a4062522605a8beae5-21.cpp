static int mov_write_mdta_ilst_tag(AVIOContext *pb, MOVMuxContext *mov,
                                   AVFormatContext *s)
{
    AVDictionaryEntry *t = NULL;
    int64_t pos = avio_tell(pb);
    int count = 1; /* keys are 1-index based */

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "ilst");

    while (t = av_dict_get(s->metadata, "", t, AV_DICT_IGNORE_SUFFIX)) {
        int64_t entry_pos = avio_tell(pb);
        avio_wb32(pb, 0); /* size */
        avio_wb32(pb, count); /* key */
        mov_write_string_data_tag(pb, t->value, 0, 1);
        update_size(pb, entry_pos);
        count += 1;
    }
    return update_size(pb, pos);
}