static av_cold int lag_decode_init(AVCodecContext *avctx)
{
    LagarithContext *l = avctx->priv_data;
    l->avctx = avctx;

    ff_dsputil_init(&l->dsp, avctx);

    return 0;
}