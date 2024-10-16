int64_t ff_guess_coded_bitrate(AVCodecContext *avctx)
{
    AVRational framerate = avctx->framerate;
    int bits_per_coded_sample = avctx->bits_per_coded_sample;
    int64_t bitrate;

    if (!(framerate.num && framerate.den))
        framerate = av_inv_q(avctx->time_base);
    if (!(framerate.num && framerate.den))
        return 0;

    if (!bits_per_coded_sample) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(avctx->pix_fmt);
        bits_per_coded_sample = av_get_bits_per_pixel(desc);
    }
    bitrate = (int64_t)bits_per_coded_sample * avctx->width * avctx->height *
              framerate.num / framerate.den;

    return bitrate;
}