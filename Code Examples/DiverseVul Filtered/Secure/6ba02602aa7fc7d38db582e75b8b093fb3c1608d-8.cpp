static av_cold int decode_end(AVCodecContext *avctx)
{
    VmncContext * const c = avctx->priv_data;

    av_frame_free(&c->pic);

    av_freep(&c->curbits);
    av_freep(&c->curmask);
    av_freep(&c->screendta);
    return 0;
}