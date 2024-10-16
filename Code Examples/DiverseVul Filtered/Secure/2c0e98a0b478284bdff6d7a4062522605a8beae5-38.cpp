static int mov_write_tfrf_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track, int entry)
{
    int n = track->nb_frag_info - 1 - entry, i;
    int size = 8 + 16 + 4 + 1 + 16*n;
    static const uint8_t uuid[] = {
        0xd4, 0x80, 0x7e, 0xf2, 0xca, 0x39, 0x46, 0x95,
        0x8e, 0x54, 0x26, 0xcb, 0x9e, 0x46, 0xa7, 0x9f
    };

    if (entry < 0)
        return 0;

    avio_seek(pb, track->frag_info[entry].tfrf_offset, SEEK_SET);
    avio_wb32(pb, size);
    ffio_wfourcc(pb, "uuid");
    avio_write(pb, uuid, sizeof(uuid));
    avio_w8(pb, 1);
    avio_wb24(pb, 0);
    avio_w8(pb, n);
    for (i = 0; i < n; i++) {
        int index = entry + 1 + i;
        avio_wb64(pb, track->frag_info[index].time);
        avio_wb64(pb, track->frag_info[index].duration);
    }
    if (n < mov->ism_lookahead) {
        int free_size = 16 * (mov->ism_lookahead - n);
        avio_wb32(pb, free_size);
        ffio_wfourcc(pb, "free");
        ffio_fill(pb, 0, free_size - 8);
    }

    return 0;
}