int attribute_align_arg avcodec_send_frame(AVCodecContext *avctx, const AVFrame *frame)
{
    if (!avcodec_is_open(avctx) || !av_codec_is_encoder(avctx->codec))
        return AVERROR(EINVAL);

    if (avctx->internal->draining)
        return AVERROR_EOF;

    if (!frame) {
        avctx->internal->draining = 1;

        if (!(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))
            return 0;
    }

    if (avctx->codec->send_frame)
        return avctx->codec->send_frame(avctx, frame);

    // Emulation via old API. Do it here instead of avcodec_receive_packet, because:
    // 1. if the AVFrame is not refcounted, the copying will be much more
    //    expensive than copying the packet data
    // 2. assume few users use non-refcounted AVPackets, so usually no copy is
    //    needed

    if (avctx->internal->buffer_pkt_valid)
        return AVERROR(EAGAIN);

    return do_encode(avctx, frame, &(int){0});
}