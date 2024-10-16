int ff_thread_ref_frame(ThreadFrame *dst, ThreadFrame *src)
{
    int ret;

    dst->owner[0] = src->owner[0];
    dst->owner[1] = src->owner[1];

    ret = av_frame_ref(dst->f, src->f);
    if (ret < 0)
        return ret;

    av_assert0(!dst->progress);

    if (src->progress &&
        !(dst->progress = av_buffer_ref(src->progress))) {
        ff_thread_release_buffer(dst->owner[0], dst);
        return AVERROR(ENOMEM);
    }

    return 0;
}