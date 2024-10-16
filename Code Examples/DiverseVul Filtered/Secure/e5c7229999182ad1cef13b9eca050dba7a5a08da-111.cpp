int ff_alloc_packet2(AVCodecContext *avctx, AVPacket *avpkt, int64_t size)
{
    if (avpkt->size < 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid negative user packet size %d\n", avpkt->size);
        return AVERROR(EINVAL);
    }
    if (size < 0 || size > INT_MAX - FF_INPUT_BUFFER_PADDING_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "Invalid minimum required packet size %"PRId64" (max allowed is %d)\n",
               size, INT_MAX - FF_INPUT_BUFFER_PADDING_SIZE);
        return AVERROR(EINVAL);
    }

    if (avctx) {
        av_assert0(!avpkt->data || avpkt->data != avctx->internal->byte_buffer);
        if (!avpkt->data || avpkt->size < size) {
            av_fast_padded_malloc(&avctx->internal->byte_buffer, &avctx->internal->byte_buffer_size, size);
            avpkt->data = avctx->internal->byte_buffer;
            avpkt->size = avctx->internal->byte_buffer_size;
            avpkt->destruct = NULL;
        }
    }

    if (avpkt->data) {
        AVBufferRef *buf = avpkt->buf;
#if FF_API_DESTRUCT_PACKET
FF_DISABLE_DEPRECATION_WARNINGS
        void *destruct = avpkt->destruct;
FF_ENABLE_DEPRECATION_WARNINGS
#endif

        if (avpkt->size < size) {
            av_log(avctx, AV_LOG_ERROR, "User packet is too small (%d < %"PRId64")\n", avpkt->size, size);
            return AVERROR(EINVAL);
        }

        av_init_packet(avpkt);
#if FF_API_DESTRUCT_PACKET
FF_DISABLE_DEPRECATION_WARNINGS
        avpkt->destruct = destruct;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
        avpkt->buf      = buf;
        avpkt->size     = size;
        return 0;
    } else {
        int ret = av_new_packet(avpkt, size);
        if (ret < 0)
            av_log(avctx, AV_LOG_ERROR, "Failed to allocate packet of size %"PRId64"\n", size);
        return ret;
    }
}