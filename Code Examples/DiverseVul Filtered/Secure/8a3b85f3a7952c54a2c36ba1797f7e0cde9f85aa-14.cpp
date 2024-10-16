static int execute_decode_slices(H264Context *h, int context_count)
{
    AVCodecContext *const avctx = h->avctx;
    H264Context *hx;
    int i;

    av_assert0(h->mb_y < h->mb_height);

    if (h->avctx->hwaccel ||
        h->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
        return 0;
    if (context_count == 1) {
        return decode_slice(avctx, &h);
    } else {
        av_assert0(context_count > 0);
        for (i = 1; i < context_count; i++) {
            hx                 = h->thread_context[i];
            if (CONFIG_ERROR_RESILIENCE) {
                hx->er.error_count = 0;
            }
            hx->x264_build     = h->x264_build;
        }

        avctx->execute(avctx, decode_slice, h->thread_context,
                       NULL, context_count, sizeof(void *));

        /* pull back stuff from slices to master context */
        hx                   = h->thread_context[context_count - 1];
        h->mb_x              = hx->mb_x;
        h->mb_y              = hx->mb_y;
        h->droppable         = hx->droppable;
        h->picture_structure = hx->picture_structure;
        if (CONFIG_ERROR_RESILIENCE) {
            for (i = 1; i < context_count; i++)
                h->er.error_count += h->thread_context[i]->er.error_count;
        }
    }

    return 0;
}