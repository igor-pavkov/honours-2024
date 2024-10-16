static av_cold int flashsv2_decode_init(AVCodecContext *avctx)
{
    FlashSVContext *s = avctx->priv_data;
    flashsv_decode_init(avctx);
    s->pal = ff_flashsv2_default_palette;
    s->ver = 2;

    return 0;
}