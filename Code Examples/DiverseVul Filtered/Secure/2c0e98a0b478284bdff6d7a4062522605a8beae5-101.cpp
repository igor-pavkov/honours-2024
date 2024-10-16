static AVRational find_fps(AVFormatContext *s, AVStream *st)
{
    AVRational rate = st->avg_frame_rate;

#if FF_API_LAVF_AVCTX
    FF_DISABLE_DEPRECATION_WARNINGS
    rate = av_inv_q(st->codec->time_base);
    if (av_timecode_check_frame_rate(rate) < 0) {
        av_log(s, AV_LOG_DEBUG, "timecode: tbc=%d/%d invalid, fallback on %d/%d\n",
               rate.num, rate.den, st->avg_frame_rate.num, st->avg_frame_rate.den);
        rate = st->avg_frame_rate;
    }
    FF_ENABLE_DEPRECATION_WARNINGS
#endif

    return rate;
}