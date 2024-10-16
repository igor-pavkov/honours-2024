int attribute_align_arg avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt)
{
    int ret;

    if (!avcodec_is_open(avctx) || !av_codec_is_decoder(avctx->codec))
        return AVERROR(EINVAL);

    if (avctx->internal->draining)
        return AVERROR_EOF;

    if (avpkt && !avpkt->size && avpkt->data)
        return AVERROR(EINVAL);

    if (!avpkt || !avpkt->size) {
        avctx->internal->draining = 1;
        avpkt = NULL;

        if (!(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))
            return 0;
    }

    if (avctx->codec->send_packet) {
        if (avpkt) {
            AVPacket tmp = *avpkt;
            int did_split = av_packet_split_side_data(&tmp);
            ret = apply_param_change(avctx, &tmp);
            if (ret >= 0)
                ret = avctx->codec->send_packet(avctx, &tmp);
            if (did_split)
                av_packet_free_side_data(&tmp);
            return ret;
        } else {
            return avctx->codec->send_packet(avctx, NULL);
        }
    }

    // Emulation via old API. Assume avpkt is likely not refcounted, while
    // decoder output is always refcounted, and avoid copying.

    if (avctx->internal->buffer_pkt->size || avctx->internal->buffer_frame->buf[0])
        return AVERROR(EAGAIN);

    // The goal is decoding the first frame of the packet without using memcpy,
    // because the common case is having only 1 frame per packet (especially
    // with video, but audio too). In other cases, it can't be avoided, unless
    // the user is feeding refcounted packets.
    return do_decode(avctx, (AVPacket *)avpkt);
}