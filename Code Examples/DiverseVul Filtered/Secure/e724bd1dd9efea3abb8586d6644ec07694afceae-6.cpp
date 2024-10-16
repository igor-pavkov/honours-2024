static av_cold int decode_end(AVCodecContext *avctx)
{
    UtvideoContext * const c = avctx->priv_data;

    av_freep(&c->slice_bits);

    return 0;
}