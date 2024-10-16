static av_cold int cmv_decode_init(AVCodecContext *avctx){
    CmvContext *s = avctx->priv_data;
    s->avctx = avctx;
    avctx->pix_fmt = AV_PIX_FMT_PAL8;

    s->last_frame  = av_frame_alloc();
    s->last2_frame = av_frame_alloc();
    if (!s->last_frame || !s->last2_frame) {
        av_frame_free(&s->last_frame);
        av_frame_free(&s->last2_frame);
        return AVERROR(ENOMEM);
    }

    return 0;
}