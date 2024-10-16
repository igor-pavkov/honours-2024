static void coroutine_fn mirror_write_complete(MirrorOp *op, int ret)
{
    MirrorBlockJob *s = op->s;

    if (ret < 0) {
        BlockErrorAction action;

        bdrv_set_dirty_bitmap(s->dirty_bitmap, op->offset, op->bytes);
        action = mirror_error_action(s, false, -ret);
        if (action == BLOCK_ERROR_ACTION_REPORT && s->ret >= 0) {
            s->ret = ret;
        }
    }

    mirror_iteration_done(op, ret);
}