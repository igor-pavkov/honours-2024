static av_cold int flashsv2_decode_end(AVCodecContext *avctx)
{
    FlashSVContext *s = avctx->priv_data;

    av_freep(&s->keyframedata);
    av_freep(&s->blocks);
    av_freep(&s->keyframe);
    av_freep(&s->deflate_block);
    flashsv_decode_end(avctx);

    return 0;
}