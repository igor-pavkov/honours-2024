static int64_t mov_get_skip_samples(AVStream *st, int sample)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int64_t first_ts = sti->index_entries[0].timestamp;
    int64_t ts = sti->index_entries[sample].timestamp;
    int64_t off;

    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
        return 0;

    /* compute skip samples according to stream start_pad, seek ts and first ts */
    off = av_rescale_q(ts - first_ts, st->time_base,
                       (AVRational){1, st->codecpar->sample_rate});
    return FFMAX(sc->start_pad - off, 0);
}