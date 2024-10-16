static void ape_flush(AVCodecContext *avctx)
{
    APEContext *s = avctx->priv_data;
    s->samples= 0;
}