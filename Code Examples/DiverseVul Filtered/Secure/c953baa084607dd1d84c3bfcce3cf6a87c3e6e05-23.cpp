static int read_tfra(MOVContext *mov, AVIOContext *f)
{
    int version, fieldlength, i, j;
    int64_t pos = avio_tell(f);
    uint32_t size = avio_rb32(f);
    unsigned track_id, item_count;

    if (avio_rb32(f) != MKBETAG('t', 'f', 'r', 'a')) {
        return 1;
    }
    av_log(mov->fc, AV_LOG_VERBOSE, "found tfra\n");

    version = avio_r8(f);
    avio_rb24(f);
    track_id = avio_rb32(f);
    fieldlength = avio_rb32(f);
    item_count = avio_rb32(f);
    for (i = 0; i < item_count; i++) {
        int64_t time, offset;
        int index;
        MOVFragmentStreamInfo * frag_stream_info;

        if (avio_feof(f)) {
            return AVERROR_INVALIDDATA;
        }

        if (version == 1) {
            time   = avio_rb64(f);
            offset = avio_rb64(f);
        } else {
            time   = avio_rb32(f);
            offset = avio_rb32(f);
        }

        // The first sample of each stream in a fragment is always a random
        // access sample.  So it's entry in the tfra can be used as the
        // initial PTS of the fragment.
        index = update_frag_index(mov, offset);
        frag_stream_info = get_frag_stream_info(&mov->frag_index, index, track_id);
        if (frag_stream_info &&
            frag_stream_info->first_tfra_pts == AV_NOPTS_VALUE)
            frag_stream_info->first_tfra_pts = time;

        for (j = 0; j < ((fieldlength >> 4) & 3) + 1; j++)
            avio_r8(f);
        for (j = 0; j < ((fieldlength >> 2) & 3) + 1; j++)
            avio_r8(f);
        for (j = 0; j < ((fieldlength >> 0) & 3) + 1; j++)
            avio_r8(f);
    }

    avio_seek(f, pos + size, SEEK_SET);
    return 0;
}