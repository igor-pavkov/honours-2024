av_cold int ff_codec_close_recursive(AVCodecContext *avctx)
{
    int ret = 0;

    ff_unlock_avcodec();

    ret = avcodec_close(avctx);

    ff_lock_avcodec(NULL);
    return ret;
}