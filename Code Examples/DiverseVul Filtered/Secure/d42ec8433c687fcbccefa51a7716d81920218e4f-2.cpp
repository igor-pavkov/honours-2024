static av_cold int decode_close(AVCodecContext *avctx)
{
    AnsiContext *s = avctx->priv_data;

    av_frame_free(&s->frame);
    return 0;
}