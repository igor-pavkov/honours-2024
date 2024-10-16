static void coroutine_fn mirror_read_complete(MirrorOp *op, int ret)
{
    MirrorBlockJob *s = op->s;

    if (ret < 0) {
        BlockErrorAction action;

        bdrv_set_dirty_bitmap(s->dirty_bitmap, op->offset, op->bytes);
        action = mirror_error_action(s, true, -ret);
        if (action == BLOCK_ERROR_ACTION_REPORT && s->ret >= 0) {
            s->ret = ret;
        }

        mirror_iteration_done(op, ret);
        return;
    }

    ret = blk_co_pwritev(s->target, op->offset, op->qiov.size, &op->qiov, 0);
    mirror_write_complete(op, ret);
}