static int mov_write_trkn_tag(AVIOContext *pb, MOVMuxContext *mov,
                              AVFormatContext *s, int disc)
{
    AVDictionaryEntry *t = av_dict_get(s->metadata,
                                       disc ? "disc" : "track",
                                       NULL, 0);
    int size = 0, track = t ? atoi(t->value) : 0;
    if (track) {
        int tracks = 0;
        char *slash = strchr(t->value, '/');
        if (slash)
            tracks = atoi(slash + 1);
        avio_wb32(pb, 32); /* size */
        ffio_wfourcc(pb, disc ? "disk" : "trkn");
        avio_wb32(pb, 24); /* size */
        ffio_wfourcc(pb, "data");
        avio_wb32(pb, 0);        // 8 bytes empty
        avio_wb32(pb, 0);
        avio_wb16(pb, 0);        // empty
        avio_wb16(pb, track);    // track / disc number
        avio_wb16(pb, tracks);   // total track / disc number
        avio_wb16(pb, 0);        // empty
        size = 32;
    }
    return size;
}