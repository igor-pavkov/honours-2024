AVHWAccel *ff_find_hwaccel(AVCodecContext *avctx)
{
    enum AVCodecID codec_id = avctx->codec->id;
    enum AVPixelFormat pix_fmt = avctx->pix_fmt;

    AVHWAccel *hwaccel = NULL;

    while ((hwaccel = av_hwaccel_next(hwaccel)))
        if (hwaccel->id == codec_id
            && hwaccel->pix_fmt == pix_fmt)
            return hwaccel;
    return NULL;
}