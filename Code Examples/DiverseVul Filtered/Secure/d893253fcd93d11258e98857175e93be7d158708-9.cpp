static int init_thread_copy(AVCodecContext *avctx)
{
    FFV1Context *f = avctx->priv_data;
    int i, ret;

    f->picture.f      = NULL;
    f->last_picture.f = NULL;
    f->sample_buffer  = NULL;
    f->max_slice_count = 0;
    f->slice_count = 0;

    for (i = 0; i < f->quant_table_count; i++) {
        av_assert0(f->version > 1);
        f->initial_states[i] = av_memdup(f->initial_states[i],
                                         f->context_count[i] * sizeof(*f->initial_states[i]));
    }

    f->picture.f      = av_frame_alloc();
    f->last_picture.f = av_frame_alloc();

    if ((ret = ffv1_init_slice_contexts(f)) < 0)
        return ret;

    return 0;
}