static int update_thread_context(AVCodecContext *dst, const AVCodecContext *src)
{
    FFV1Context *fsrc = src->priv_data;
    FFV1Context *fdst = dst->priv_data;
    int i, ret;

    if (dst == src)
        return 0;

    {
        FFV1Context bak = *fdst;
        memcpy(fdst, fsrc, sizeof(*fdst));
        memcpy(fdst->initial_states, bak.initial_states, sizeof(fdst->initial_states));
        memcpy(fdst->slice_context,  bak.slice_context , sizeof(fdst->slice_context));
        fdst->picture      = bak.picture;
        fdst->last_picture = bak.last_picture;
        for (i = 0; i<fdst->num_h_slices * fdst->num_v_slices; i++) {
            FFV1Context *fssrc = fsrc->slice_context[i];
            FFV1Context *fsdst = fdst->slice_context[i];
            copy_fields(fsdst, fssrc, fsrc);
        }
        av_assert0(!fdst->plane[0].state);
        av_assert0(!fdst->sample_buffer);
    }

    av_assert1(fdst->max_slice_count == fsrc->max_slice_count);


    ff_thread_release_buffer(dst, &fdst->picture);
    if (fsrc->picture.f->data[0]) {
        if ((ret = ff_thread_ref_frame(&fdst->picture, &fsrc->picture)) < 0)
            return ret;
    }

    fdst->fsrc = fsrc;

    return 0;
}