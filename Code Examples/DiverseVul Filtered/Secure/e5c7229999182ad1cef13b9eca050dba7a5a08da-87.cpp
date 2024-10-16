int avcodec_default_get_buffer(AVCodecContext *avctx, AVFrame *frame)
{
    return avcodec_default_get_buffer2(avctx, frame, 0);
}