static int mirror_flush(MirrorBlockJob *s)
{
    int ret = blk_flush(s->target);
    if (ret < 0) {
        if (mirror_error_action(s, false, -ret) == BLOCK_ERROR_ACTION_REPORT) {
            s->ret = ret;
        }
    }
    return ret;
}