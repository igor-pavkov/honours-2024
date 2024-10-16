static int setup_hwaccel(AVCodecContext *avctx,
                         const enum AVPixelFormat fmt,
                         const char *name)
{
    AVHWAccel *hwa = find_hwaccel(avctx->codec_id, fmt);
    int ret        = 0;

    if (avctx->active_thread_type & FF_THREAD_FRAME) {
        av_log(avctx, AV_LOG_WARNING,
               "Hardware accelerated decoding with frame threading is known to be unstable and its use is discouraged.\n");
    }

    if (!hwa) {
        av_log(avctx, AV_LOG_ERROR,
               "Could not find an AVHWAccel for the pixel format: %s",
               name);
        return AVERROR(ENOENT);
    }

    if (hwa->capabilities & HWACCEL_CODEC_CAP_EXPERIMENTAL &&
        avctx->strict_std_compliance > FF_COMPLIANCE_EXPERIMENTAL) {
        av_log(avctx, AV_LOG_WARNING, "Ignoring experimental hwaccel: %s\n",
               hwa->name);
        return AVERROR_PATCHWELCOME;
    }

    if (hwa->priv_data_size) {
        avctx->internal->hwaccel_priv_data = av_mallocz(hwa->priv_data_size);
        if (!avctx->internal->hwaccel_priv_data)
            return AVERROR(ENOMEM);
    }

    if (hwa->init) {
        ret = hwa->init(avctx);
        if (ret < 0) {
            av_freep(&avctx->internal->hwaccel_priv_data);
            return ret;
        }
    }

    avctx->hwaccel = hwa;

    return 0;
}