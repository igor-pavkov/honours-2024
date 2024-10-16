static av_cold int latm_decode_init(AVCodecContext *avctx)
{
    struct LATMContext *latmctx = avctx->priv_data;
    int ret = aac_decode_init(avctx);

    if (avctx->extradata_size > 0)
        latmctx->initialized = !ret;

    return ret;
}