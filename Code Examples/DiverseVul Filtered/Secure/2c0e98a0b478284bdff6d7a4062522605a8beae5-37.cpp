static int mov_write_sidx_tags(AVIOContext *pb, MOVMuxContext *mov,
                               int tracks, int ref_size)
{
    int i, round, ret;
    AVIOContext *avio_buf;
    int total_size = 0;
    for (round = 0; round < 2; round++) {
        // First run one round to calculate the total size of all
        // sidx atoms.
        // This would be much simpler if we'd only write one sidx
        // atom, for the first track in the moof.
        if (round == 0) {
            if ((ret = ffio_open_null_buf(&avio_buf)) < 0)
                return ret;
        } else {
            avio_buf = pb;
        }
        for (i = 0; i < mov->nb_streams; i++) {
            MOVTrack *track = &mov->tracks[i];
            if (tracks >= 0 && i != tracks)
                continue;
            // When writing a sidx for the full file, entry is 0, but
            // we want to include all tracks. ref_size is 0 in this case,
            // since we read it from frag_info instead.
            if (!track->entry && ref_size > 0)
                continue;
            total_size -= mov_write_sidx_tag(avio_buf, track, ref_size,
                                             total_size);
        }
        if (round == 0)
            total_size = ffio_close_null_buf(avio_buf);
    }
    return 0;
}