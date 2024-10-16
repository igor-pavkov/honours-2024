static av_cold int decode_init(AVCodecContext *avctx)
{
    Mpeg4DecContext *ctx = avctx->priv_data;
    MpegEncContext *s = &ctx->m;
    int ret;

    ctx->divx_version =
    ctx->divx_build   =
    ctx->xvid_build   =
    ctx->lavc_build   = -1;

    if ((ret = ff_h263_decode_init(avctx)) < 0)
        return ret;

    ff_mpeg4videodec_static_init();

    s->h263_pred = 1;
    s->low_delay = 0; /* default, might be overridden in the vol header during header parsing */
    s->decode_mb = mpeg4_decode_mb;
    ctx->time_increment_bits = 4; /* default value for broken headers */

    avctx->chroma_sample_location = AVCHROMA_LOC_LEFT;
    avctx->internal->allocate_progress = 1;

    return 0;
}