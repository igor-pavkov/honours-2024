int avcodec_default_get_buffer2(AVCodecContext *avctx, AVFrame *frame, int flags)
{
    int ret;

    if ((ret = update_frame_pool(avctx, frame)) < 0)
        return ret;

#if FF_API_GET_BUFFER
FF_DISABLE_DEPRECATION_WARNINGS
    frame->type = FF_BUFFER_TYPE_INTERNAL;
FF_ENABLE_DEPRECATION_WARNINGS
#endif

    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        return video_get_buffer(avctx, frame);
    case AVMEDIA_TYPE_AUDIO:
        return audio_get_buffer(avctx, frame);
    default:
        return -1;
    }
}