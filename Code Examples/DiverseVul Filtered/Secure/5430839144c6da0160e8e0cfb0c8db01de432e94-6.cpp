static av_cold int cmv_decode_end(AVCodecContext *avctx){
    CmvContext *s = avctx->priv_data;

    av_frame_free(&s->last_frame);
    av_frame_free(&s->last2_frame);

    return 0;
}