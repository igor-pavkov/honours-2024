static av_cold void flush(AVCodecContext *avctx)
{
    ALSDecContext *ctx = avctx->priv_data;

    ctx->frame_id = 0;
}