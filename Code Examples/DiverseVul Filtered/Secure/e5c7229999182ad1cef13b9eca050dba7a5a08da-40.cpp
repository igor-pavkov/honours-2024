static int get_buffer_internal(AVCodecContext *avctx, AVFrame *frame, int flags)
{
    int override_dimensions = 1;
    int ret;

    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        if ((ret = av_image_check_size(avctx->width, avctx->height, 0, avctx)) < 0 || avctx->pix_fmt<0) {
            av_log(avctx, AV_LOG_ERROR, "video_get_buffer: image parameters invalid\n");
            return AVERROR(EINVAL);
        }
    }
    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (frame->width <= 0 || frame->height <= 0) {
            frame->width  = FFMAX(avctx->width,  FF_CEIL_RSHIFT(avctx->coded_width,  avctx->lowres));
            frame->height = FFMAX(avctx->height, FF_CEIL_RSHIFT(avctx->coded_height, avctx->lowres));
            override_dimensions = 0;
        }
    }
    if ((ret = ff_init_buffer_info(avctx, frame)) < 0)
        return ret;

#if FF_API_GET_BUFFER
FF_DISABLE_DEPRECATION_WARNINGS
    /*
     * Wrap an old get_buffer()-allocated buffer in a bunch of AVBuffers.
     * We wrap each plane in its own AVBuffer. Each of those has a reference to
     * a dummy AVBuffer as its private data, unreffing it on free.
     * When all the planes are freed, the dummy buffer's free callback calls
     * release_buffer().
     */
    if (avctx->get_buffer) {
        CompatReleaseBufPriv *priv = NULL;
        AVBufferRef *dummy_buf = NULL;
        int planes, i, ret;

        if (flags & AV_GET_BUFFER_FLAG_REF)
            frame->reference    = 1;

        ret = avctx->get_buffer(avctx, frame);
        if (ret < 0)
            return ret;

        /* return if the buffers are already set up
         * this would happen e.g. when a custom get_buffer() calls
         * avcodec_default_get_buffer
         */
        if (frame->buf[0])
            goto end;

        priv = av_mallocz(sizeof(*priv));
        if (!priv) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        priv->avctx = *avctx;
        priv->frame = *frame;

        dummy_buf = av_buffer_create(NULL, 0, compat_free_buffer, priv, 0);
        if (!dummy_buf) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }

#define WRAP_PLANE(ref_out, data, data_size)                            \
do {                                                                    \
    AVBufferRef *dummy_ref = av_buffer_ref(dummy_buf);                  \
    if (!dummy_ref) {                                                   \
        ret = AVERROR(ENOMEM);                                          \
        goto fail;                                                      \
    }                                                                   \
    ref_out = av_buffer_create(data, data_size, compat_release_buffer,  \
                               dummy_ref, 0);                           \
    if (!ref_out) {                                                     \
        av_frame_unref(frame);                                          \
        ret = AVERROR(ENOMEM);                                          \
        goto fail;                                                      \
    }                                                                   \
} while (0)

        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(frame->format);

            planes = av_pix_fmt_count_planes(frame->format);
            /* workaround for AVHWAccel plane count of 0, buf[0] is used as
               check for allocated buffers: make libavcodec happy */
            if (desc && desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
                planes = 1;
            if (!desc || planes <= 0) {
                ret = AVERROR(EINVAL);
                goto fail;
            }

            for (i = 0; i < planes; i++) {
                int v_shift    = (i == 1 || i == 2) ? desc->log2_chroma_h : 0;
                int plane_size = (frame->height >> v_shift) * frame->linesize[i];

                WRAP_PLANE(frame->buf[i], frame->data[i], plane_size);
            }
        } else {
            int planar = av_sample_fmt_is_planar(frame->format);
            planes = planar ? avctx->channels : 1;

            if (planes > FF_ARRAY_ELEMS(frame->buf)) {
                frame->nb_extended_buf = planes - FF_ARRAY_ELEMS(frame->buf);
                frame->extended_buf = av_malloc(sizeof(*frame->extended_buf) *
                                                frame->nb_extended_buf);
                if (!frame->extended_buf) {
                    ret = AVERROR(ENOMEM);
                    goto fail;
                }
            }

            for (i = 0; i < FFMIN(planes, FF_ARRAY_ELEMS(frame->buf)); i++)
                WRAP_PLANE(frame->buf[i], frame->extended_data[i], frame->linesize[0]);

            for (i = 0; i < frame->nb_extended_buf; i++)
                WRAP_PLANE(frame->extended_buf[i],
                           frame->extended_data[i + FF_ARRAY_ELEMS(frame->buf)],
                           frame->linesize[0]);
        }

        av_buffer_unref(&dummy_buf);

end:
        frame->width  = avctx->width;
        frame->height = avctx->height;

        return 0;

fail:
        avctx->release_buffer(avctx, frame);
        av_freep(&priv);
        av_buffer_unref(&dummy_buf);
        return ret;
    }
FF_ENABLE_DEPRECATION_WARNINGS
#endif

    ret = avctx->get_buffer2(avctx, frame, flags);

    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO && !override_dimensions) {
        frame->width  = avctx->width;
        frame->height = avctx->height;
    }

    return ret;
}