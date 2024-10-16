static int coroutine_fn bdrv_mirror_top_do_write(BlockDriverState *bs,
    MirrorMethod method, uint64_t offset, uint64_t bytes, QEMUIOVector *qiov,
    int flags)
{
    MirrorOp *op = NULL;
    MirrorBDSOpaque *s = bs->opaque;
    int ret = 0;
    bool copy_to_target;

    copy_to_target = s->job->ret >= 0 &&
                     s->job->copy_mode == MIRROR_COPY_MODE_WRITE_BLOCKING;

    if (copy_to_target) {
        op = active_write_prepare(s->job, offset, bytes);
    }

    switch (method) {
    case MIRROR_METHOD_COPY:
        ret = bdrv_co_pwritev(bs->backing, offset, bytes, qiov, flags);
        break;

    case MIRROR_METHOD_ZERO:
        ret = bdrv_co_pwrite_zeroes(bs->backing, offset, bytes, flags);
        break;

    case MIRROR_METHOD_DISCARD:
        ret = bdrv_co_pdiscard(bs->backing, offset, bytes);
        break;

    default:
        abort();
    }

    if (ret < 0) {
        goto out;
    }

    if (copy_to_target) {
        do_sync_target_write(s->job, method, offset, bytes, qiov, flags);
    }

out:
    if (copy_to_target) {
        active_write_settle(op);
    }
    return ret;
}