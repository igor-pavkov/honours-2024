static av_cold int shorten_decode_init(AVCodecContext *avctx)
{
    ShortenContext *s = avctx->priv_data;
    s->avctx          = avctx;
    avctx->sample_fmt = AV_SAMPLE_FMT_S16P;

    return 0;
}