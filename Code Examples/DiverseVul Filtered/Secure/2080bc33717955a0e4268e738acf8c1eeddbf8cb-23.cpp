enum AVPixelFormat ff_thread_get_format(AVCodecContext *avctx, const enum AVPixelFormat *fmt)
{
    return ff_get_format(avctx, fmt);
}