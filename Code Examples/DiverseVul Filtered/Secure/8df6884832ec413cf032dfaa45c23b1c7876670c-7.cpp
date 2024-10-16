static void ff_lock_avcodec(AVCodecContext *log_ctx, const AVCodec *codec)
{
    if (!(codec->caps_internal & FF_CODEC_CAP_INIT_THREADSAFE) && codec->init)
        ff_mutex_lock(&codec_mutex);
}