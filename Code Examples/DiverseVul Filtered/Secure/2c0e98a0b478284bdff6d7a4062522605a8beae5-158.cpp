static int mov_write_ctts_tag(AVIOContext *pb, MOVTrack *track)
{
    MOVStts *ctts_entries;
    uint32_t entries = 0;
    uint32_t atom_size;
    int i;

    ctts_entries = av_malloc_array((track->entry + 1), sizeof(*ctts_entries)); /* worst case */
    if (!ctts_entries)
        return AVERROR(ENOMEM);
    ctts_entries[0].count = 1;
    ctts_entries[0].duration = track->cluster[0].cts;
    for (i = 1; i < track->entry; i++) {
        if (track->cluster[i].cts == ctts_entries[entries].duration) {
            ctts_entries[entries].count++; /* compress */
        } else {
            entries++;
            ctts_entries[entries].duration = track->cluster[i].cts;
            ctts_entries[entries].count = 1;
        }
    }
    entries++; /* last one */
    atom_size = 16 + (entries * 8);
    avio_wb32(pb, atom_size); /* size */
    ffio_wfourcc(pb, "ctts");
    avio_wb32(pb, 0); /* version & flags */
    avio_wb32(pb, entries); /* entry count */
    for (i = 0; i < entries; i++) {
        avio_wb32(pb, ctts_entries[i].count);
        avio_wb32(pb, ctts_entries[i].duration);
    }
    av_free(ctts_entries);
    return atom_size;
}