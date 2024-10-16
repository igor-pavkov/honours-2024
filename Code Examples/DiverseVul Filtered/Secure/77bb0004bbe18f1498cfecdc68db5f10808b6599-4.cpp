static av_cold int rpza_decode_end(AVCodecContext *avctx)
{
    RpzaContext *s = avctx->priv_data;

    av_frame_free(&s->frame);

    return 0;
}