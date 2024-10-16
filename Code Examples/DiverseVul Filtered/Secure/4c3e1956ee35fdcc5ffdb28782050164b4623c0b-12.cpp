static av_cold int lag_decode_end(AVCodecContext *avctx)
{
    LagarithContext *l = avctx->priv_data;

    av_freep(&l->rgb_planes);

    return 0;
}