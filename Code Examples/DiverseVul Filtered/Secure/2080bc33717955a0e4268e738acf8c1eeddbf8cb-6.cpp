int ff_set_sar(AVCodecContext *avctx, AVRational sar)
{
    int ret = av_image_check_sar(avctx->width, avctx->height, sar);

    if (ret < 0) {
        av_log(avctx, AV_LOG_WARNING, "ignoring invalid SAR: %d/%d\n",
               sar.num, sar.den);
        avctx->sample_aspect_ratio = (AVRational){ 0, 1 };
        return ret;
    } else {
        avctx->sample_aspect_ratio = sar;
    }
    return 0;
}