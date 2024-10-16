static int mov_seek_stream(AVFormatContext *s, AVStream *st, int64_t timestamp, int flags)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int sample, time_sample, ret;
    unsigned int i;

    // Here we consider timestamp to be PTS, hence try to offset it so that we
    // can search over the DTS timeline.
    timestamp -= (sc->min_corrected_pts + sc->dts_shift);

    ret = mov_seek_fragment(s, st, timestamp);
    if (ret < 0)
        return ret;

    for (;;) {
        sample = av_index_search_timestamp(st, timestamp, flags);
        av_log(s, AV_LOG_TRACE, "stream %d, timestamp %"PRId64", sample %d\n", st->index, timestamp, sample);
        if (sample < 0 && sti->nb_index_entries && timestamp < sti->index_entries[0].timestamp)
            sample = 0;
        if (sample < 0) /* not sure what to do */
            return AVERROR_INVALIDDATA;

        if (!sample || can_seek_to_key_sample(st, sample, timestamp))
            break;
        timestamp -= FFMAX(sc->min_sample_duration, 1);
    }

    mov_current_sample_set(sc, sample);
    av_log(s, AV_LOG_TRACE, "stream %d, found sample %d\n", st->index, sc->current_sample);
    /* adjust ctts index */
    if (sc->ctts_data) {
        time_sample = 0;
        for (i = 0; i < sc->ctts_count; i++) {
            int next = time_sample + sc->ctts_data[i].count;
            if (next > sc->current_sample) {
                sc->ctts_index = i;
                sc->ctts_sample = sc->current_sample - time_sample;
                break;
            }
            time_sample = next;
        }
    }

    /* adjust stsd index */
    if (sc->chunk_count) {
        time_sample = 0;
        for (i = 0; i < sc->stsc_count; i++) {
            int64_t next = time_sample + mov_get_stsc_samples(sc, i);
            if (next > sc->current_sample) {
                sc->stsc_index = i;
                sc->stsc_sample = sc->current_sample - time_sample;
                break;
            }
            av_assert0(next == (int)next);
            time_sample = next;
        }
    }

    return sample;
}