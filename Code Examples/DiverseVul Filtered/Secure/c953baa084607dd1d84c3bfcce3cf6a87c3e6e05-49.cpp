static int mov_read_seek(AVFormatContext *s, int stream_index, int64_t sample_time, int flags)
{
    MOVContext *mc = s->priv_data;
    AVStream *st;
    FFStream *sti;
    int sample;
    int i;

    if (stream_index >= s->nb_streams)
        return AVERROR_INVALIDDATA;

    st = s->streams[stream_index];
    sti = ffstream(st);
    sample = mov_seek_stream(s, st, sample_time, flags);
    if (sample < 0)
        return sample;

    if (mc->seek_individually) {
        /* adjust seek timestamp to found sample timestamp */
        int64_t seek_timestamp = sti->index_entries[sample].timestamp;
        sti->skip_samples = mov_get_skip_samples(st, sample);

        for (i = 0; i < s->nb_streams; i++) {
            AVStream *const st  = s->streams[i];
            FFStream *const sti = ffstream(st);
            int64_t timestamp;

            if (stream_index == i)
                continue;

            timestamp = av_rescale_q(seek_timestamp, s->streams[stream_index]->time_base, st->time_base);
            sample = mov_seek_stream(s, st, timestamp, flags);
            if (sample >= 0)
                sti->skip_samples = mov_get_skip_samples(st, sample);
        }
    } else {
        for (i = 0; i < s->nb_streams; i++) {
            MOVStreamContext *sc;
            st = s->streams[i];
            sc = st->priv_data;
            mov_current_sample_set(sc, 0);
        }
        while (1) {
            MOVStreamContext *sc;
            AVIndexEntry *entry = mov_find_next_sample(s, &st);
            if (!entry)
                return AVERROR_INVALIDDATA;
            sc = st->priv_data;
            if (sc->ffindex == stream_index && sc->current_sample == sample)
                break;
            mov_current_sample_inc(sc);
        }
    }
    return 0;
}