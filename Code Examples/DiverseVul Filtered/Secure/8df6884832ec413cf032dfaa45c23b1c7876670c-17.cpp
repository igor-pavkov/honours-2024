static void ff_unlock_avcodec(const AVCodec *codec)
{
    if (!(codec->caps_internal & FF_CODEC_CAP_INIT_THREADSAFE) && codec->init)
        ff_mutex_unlock(&codec_mutex);
}