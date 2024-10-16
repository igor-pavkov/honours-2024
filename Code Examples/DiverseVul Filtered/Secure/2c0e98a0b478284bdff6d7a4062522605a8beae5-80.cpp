static int mov_write_edts_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track)
{
    int64_t duration = av_rescale_rnd(track->track_duration, MOV_TIMESCALE,
                                      track->timescale, AV_ROUND_UP);
    int version = duration < INT32_MAX ? 0 : 1;
    int entry_size, entry_count, size;
    int64_t delay, start_ct = track->start_cts;
    int64_t start_dts = track->start_dts;

    if (track->entry) {
        if (start_dts != track->cluster[0].dts || start_ct != track->cluster[0].cts) {

            av_log(mov->fc, AV_LOG_DEBUG,
                   "EDTS using dts:%"PRId64" cts:%d instead of dts:%"PRId64" cts:%"PRId64" tid:%d\n",
                   track->cluster[0].dts, track->cluster[0].cts,
                   start_dts, start_ct, track->track_id);
            start_dts = track->cluster[0].dts;
            start_ct  = track->cluster[0].cts;
        }
    }

    delay = av_rescale_rnd(start_dts + start_ct, MOV_TIMESCALE,
                           track->timescale, AV_ROUND_DOWN);
    version |= delay < INT32_MAX ? 0 : 1;

    entry_size = (version == 1) ? 20 : 12;
    entry_count = 1 + (delay > 0);
    size = 24 + entry_count * entry_size;

    /* write the atom data */
    avio_wb32(pb, size);
    ffio_wfourcc(pb, "edts");
    avio_wb32(pb, size - 8);
    ffio_wfourcc(pb, "elst");
    avio_w8(pb, version);
    avio_wb24(pb, 0); /* flags */

    avio_wb32(pb, entry_count);
    if (delay > 0) { /* add an empty edit to delay presentation */
        /* In the positive delay case, the delay includes the cts
         * offset, and the second edit list entry below trims out
         * the same amount from the actual content. This makes sure
         * that the offset last sample is included in the edit
         * list duration as well. */
        if (version == 1) {
            avio_wb64(pb, delay);
            avio_wb64(pb, -1);
        } else {
            avio_wb32(pb, delay);
            avio_wb32(pb, -1);
        }
        avio_wb32(pb, 0x00010000);
    } else {
        /* Avoid accidentally ending up with start_ct = -1 which has got a
         * special meaning. Normally start_ct should end up positive or zero
         * here, but use FFMIN in case dts is a small positive integer
         * rounded to 0 when represented in MOV_TIMESCALE units. */
        av_assert0(av_rescale_rnd(start_dts, MOV_TIMESCALE, track->timescale, AV_ROUND_DOWN) <= 0);
        start_ct  = -FFMIN(start_dts, 0);
        /* Note, this delay is calculated from the pts of the first sample,
         * ensuring that we don't reduce the duration for cases with
         * dts<0 pts=0. */
        duration += delay;
    }

    /* For fragmented files, we don't know the full length yet. Setting
     * duration to 0 allows us to only specify the offset, including
     * the rest of the content (from all future fragments) without specifying
     * an explicit duration. */
    if (mov->flags & FF_MOV_FLAG_FRAGMENT)
        duration = 0;

    /* duration */
    if (version == 1) {
        avio_wb64(pb, duration);
        avio_wb64(pb, start_ct);
    } else {
        avio_wb32(pb, duration);
        avio_wb32(pb, start_ct);
    }
    avio_wb32(pb, 0x00010000);
    return size;
}