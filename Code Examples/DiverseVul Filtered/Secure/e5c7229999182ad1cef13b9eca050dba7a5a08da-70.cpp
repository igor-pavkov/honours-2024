enum AVPixelFormat avcodec_default_get_format(struct AVCodecContext *s, const enum AVPixelFormat *fmt)
{
    while (*fmt != AV_PIX_FMT_NONE && is_hwaccel_pix_fmt(*fmt))
        ++fmt;
    return fmt[0];
}