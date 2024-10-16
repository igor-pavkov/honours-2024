static av_cold int truemotion1_decode_end(AVCodecContext *avctx)
{
    TrueMotion1Context *s = avctx->priv_data;

    av_frame_free(&s->frame);
    av_free(s->vert_pred);

    return 0;
}