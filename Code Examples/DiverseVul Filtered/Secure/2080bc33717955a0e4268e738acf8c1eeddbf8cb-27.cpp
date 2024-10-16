static AVHWAccel *find_hwaccel(enum AVCodecID codec_id,
                               enum AVPixelFormat pix_fmt)
{
    AVHWAccel *hwaccel = NULL;

    while ((hwaccel = av_hwaccel_next(hwaccel)))
        if (hwaccel->id == codec_id
            && hwaccel->pix_fmt == pix_fmt)
            return hwaccel;
    return NULL;
}