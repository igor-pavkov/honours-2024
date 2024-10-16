static int mov_write_moof_tag_internal(AVIOContext *pb, MOVMuxContext *mov,
                                       int tracks, int moof_size)
{
    int64_t pos = avio_tell(pb);
    int i;

    avio_wb32(pb, 0); /* size placeholder */
    ffio_wfourcc(pb, "moof");
    mov->first_trun = 1;

    mov_write_mfhd_tag(pb, mov);
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if (tracks >= 0 && i != tracks)
            continue;
        if (!track->entry)
            continue;
        mov_write_traf_tag(pb, mov, track, pos, moof_size);
    }

    return update_size(pb, pos);
}