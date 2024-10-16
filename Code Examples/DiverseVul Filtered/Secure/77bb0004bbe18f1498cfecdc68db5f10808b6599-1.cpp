static av_cold int rpza_decode_init(AVCodecContext *avctx)
{
    RpzaContext *s = avctx->priv_data;

    s->avctx = avctx;
    avctx->pix_fmt = AV_PIX_FMT_RGB555;

    s->frame = av_frame_alloc();
    if (!s->frame)
        return AVERROR(ENOMEM);

    return 0;
}