static int mov_write_trun_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track, int moof_size,
                              int first, int end)
{
    int64_t pos = avio_tell(pb);
    uint32_t flags = MOV_TRUN_DATA_OFFSET;
    int i;

    for (i = first; i < end; i++) {
        if (get_cluster_duration(track, i) != track->default_duration)
            flags |= MOV_TRUN_SAMPLE_DURATION;
        if (track->cluster[i].size != track->default_size)
            flags |= MOV_TRUN_SAMPLE_SIZE;
        if (i > first && get_sample_flags(track, &track->cluster[i]) != track->default_sample_flags)
            flags |= MOV_TRUN_SAMPLE_FLAGS;
    }
    if (!(flags & MOV_TRUN_SAMPLE_FLAGS) && track->entry > 0 &&
         get_sample_flags(track, &track->cluster[0]) != track->default_sample_flags)
        flags |= MOV_TRUN_FIRST_SAMPLE_FLAGS;
    if (track->flags & MOV_TRACK_CTTS)
        flags |= MOV_TRUN_SAMPLE_CTS;

    avio_wb32(pb, 0); /* size placeholder */
    ffio_wfourcc(pb, "trun");
    avio_w8(pb, 0); /* version */
    avio_wb24(pb, flags);

    avio_wb32(pb, end - first); /* sample count */
    if (mov->flags & FF_MOV_FLAG_OMIT_TFHD_OFFSET &&
        !(mov->flags & FF_MOV_FLAG_DEFAULT_BASE_MOOF) &&
        !mov->first_trun)
        avio_wb32(pb, 0); /* Later tracks follow immediately after the previous one */
    else
        avio_wb32(pb, moof_size + 8 + track->data_offset +
                      track->cluster[first].pos); /* data offset */
    if (flags & MOV_TRUN_FIRST_SAMPLE_FLAGS)
        avio_wb32(pb, get_sample_flags(track, &track->cluster[first]));

    for (i = first; i < end; i++) {
        if (flags & MOV_TRUN_SAMPLE_DURATION)
            avio_wb32(pb, get_cluster_duration(track, i));
        if (flags & MOV_TRUN_SAMPLE_SIZE)
            avio_wb32(pb, track->cluster[i].size);
        if (flags & MOV_TRUN_SAMPLE_FLAGS)
            avio_wb32(pb, get_sample_flags(track, &track->cluster[i]));
        if (flags & MOV_TRUN_SAMPLE_CTS)
            avio_wb32(pb, track->cluster[i].cts);
    }

    mov->first_trun = 0;
    return update_size(pb, pos);
}