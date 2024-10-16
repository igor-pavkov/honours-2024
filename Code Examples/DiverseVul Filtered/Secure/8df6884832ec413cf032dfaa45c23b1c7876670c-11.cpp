int ff_thread_get_buffer(AVCodecContext *avctx, ThreadFrame *f, int flags)
{
    f->owner[0] = f->owner[1] = avctx;
    return ff_get_buffer(avctx, f->f, flags);
}