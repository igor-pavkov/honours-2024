static int64_t mxf_compute_sample_count(MXFContext *mxf, AVStream *st,
                                        int64_t edit_unit)
{
    int i, total = 0, size = 0;
    MXFTrack *track = st->priv_data;
    AVRational time_base = av_inv_q(track->edit_rate);
    AVRational sample_rate = av_inv_q(st->time_base);
    const MXFSamplesPerFrame *spf = NULL;
    int64_t sample_count;

    // For non-audio sample_count equals current edit unit
    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
        return edit_unit;

    if ((sample_rate.num / sample_rate.den) == 48000)
        spf = ff_mxf_get_samples_per_frame(mxf->fc, time_base);
    if (!spf) {
        int remainder = (sample_rate.num * time_base.num) %
                        (time_base.den * sample_rate.den);
        if (remainder)
            av_log(mxf->fc, AV_LOG_WARNING,
                   "seeking detected on stream #%d with time base (%d/%d) and "
                   "sample rate (%d/%d), audio pts won't be accurate.\n",
                   st->index, time_base.num, time_base.den,
                   sample_rate.num, sample_rate.den);
        return av_rescale_q(edit_unit, sample_rate, track->edit_rate);
    }

    while (spf->samples_per_frame[size]) {
        total += spf->samples_per_frame[size];
        size++;
    }

    av_assert2(size);

    sample_count = (edit_unit / size) * (uint64_t)total;
    for (i = 0; i < edit_unit % size; i++) {
        sample_count += spf->samples_per_frame[i];
    }

    return sample_count;
}