static int coroutine_fn bdrv_mirror_top_pwritev(BlockDriverState *bs,
    uint64_t offset, uint64_t bytes, QEMUIOVector *qiov, int flags)
{
    MirrorBDSOpaque *s = bs->opaque;
    QEMUIOVector bounce_qiov;
    void *bounce_buf;
    int ret = 0;
    bool copy_to_target;

    copy_to_target = s->job->ret >= 0 &&
                     s->job->copy_mode == MIRROR_COPY_MODE_WRITE_BLOCKING;

    if (copy_to_target) {
        /* The guest might concurrently modify the data to write; but
         * the data on source and destination must match, so we have
         * to use a bounce buffer if we are going to write to the
         * target now. */
        bounce_buf = qemu_blockalign(bs, bytes);
        iov_to_buf_full(qiov->iov, qiov->niov, 0, bounce_buf, bytes);

        qemu_iovec_init(&bounce_qiov, 1);
        qemu_iovec_add(&bounce_qiov, bounce_buf, bytes);
        qiov = &bounce_qiov;
    }

    ret = bdrv_mirror_top_do_write(bs, MIRROR_METHOD_COPY, offset, bytes, qiov,
                                   flags);

    if (copy_to_target) {
        qemu_iovec_destroy(&bounce_qiov);
        qemu_vfree(bounce_buf);
    }

    return ret;
}