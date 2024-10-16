av_cold int avcodec_close(AVCodecContext *avctx)
{
    int ret;

    if (!avctx)
        return 0;

    ret = ff_lock_avcodec(avctx);
    if (ret < 0)
        return ret;

    if (avcodec_is_open(avctx)) {
        FramePool *pool = avctx->internal->pool;
        int i;
        if (CONFIG_FRAME_THREAD_ENCODER &&
            avctx->internal->frame_thread_encoder && avctx->thread_count > 1) {
            ff_unlock_avcodec();
            ff_frame_thread_encoder_free(avctx);
            ff_lock_avcodec(avctx);
        }
        if (HAVE_THREADS && avctx->internal->thread_ctx)
            ff_thread_free(avctx);
        if (avctx->codec && avctx->codec->close)
            avctx->codec->close(avctx);
        avctx->coded_frame = NULL;
        avctx->internal->byte_buffer_size = 0;
        av_freep(&avctx->internal->byte_buffer);
        av_frame_free(&avctx->internal->to_free);
        for (i = 0; i < FF_ARRAY_ELEMS(pool->pools); i++)
            av_buffer_pool_uninit(&pool->pools[i]);
        av_freep(&avctx->internal->pool);
        av_freep(&avctx->internal);
    }

    if (avctx->priv_data && avctx->codec && avctx->codec->priv_class)
        av_opt_free(avctx->priv_data);
    av_opt_free(avctx);
    av_freep(&avctx->priv_data);
    if (av_codec_is_encoder(avctx->codec))
        av_freep(&avctx->extradata);
    avctx->codec = NULL;
    avctx->active_thread_type = 0;

    ff_unlock_avcodec();
    return 0;
}