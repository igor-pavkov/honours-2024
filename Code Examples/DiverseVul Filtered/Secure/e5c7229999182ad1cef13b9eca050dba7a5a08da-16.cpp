static int audio_get_buffer(AVCodecContext *avctx, AVFrame *frame)
{
    FramePool *pool = avctx->internal->pool;
    int planes = pool->planes;
    int i;

    frame->linesize[0] = pool->linesize[0];

    if (planes > AV_NUM_DATA_POINTERS) {
        frame->extended_data = av_mallocz(planes * sizeof(*frame->extended_data));
        frame->nb_extended_buf = planes - AV_NUM_DATA_POINTERS;
        frame->extended_buf  = av_mallocz(frame->nb_extended_buf *
                                          sizeof(*frame->extended_buf));
        if (!frame->extended_data || !frame->extended_buf) {
            av_freep(&frame->extended_data);
            av_freep(&frame->extended_buf);
            return AVERROR(ENOMEM);
        }
    } else {
        frame->extended_data = frame->data;
        av_assert0(frame->nb_extended_buf == 0);
    }

    for (i = 0; i < FFMIN(planes, AV_NUM_DATA_POINTERS); i++) {
        frame->buf[i] = av_buffer_pool_get(pool->pools[0]);
        if (!frame->buf[i])
            goto fail;
        frame->extended_data[i] = frame->data[i] = frame->buf[i]->data;
    }
    for (i = 0; i < frame->nb_extended_buf; i++) {
        frame->extended_buf[i] = av_buffer_pool_get(pool->pools[0]);
        if (!frame->extended_buf[i])
            goto fail;
        frame->extended_data[i + AV_NUM_DATA_POINTERS] = frame->extended_buf[i]->data;
    }

    if (avctx->debug & FF_DEBUG_BUFFERS)
        av_log(avctx, AV_LOG_DEBUG, "default_get_buffer called on frame %p", frame);

    return 0;
fail:
    av_frame_unref(frame);
    return AVERROR(ENOMEM);
}