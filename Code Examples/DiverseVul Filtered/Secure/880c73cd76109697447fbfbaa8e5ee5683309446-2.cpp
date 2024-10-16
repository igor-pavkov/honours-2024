static av_cold int flashsv_decode_end(AVCodecContext *avctx)
{
    FlashSVContext *s = avctx->priv_data;
    inflateEnd(&s->zstream);
    /* release the frame if needed */
    av_frame_unref(&s->frame);

    /* free the tmpblock */
    av_free(s->tmpblock);

    return 0;
}