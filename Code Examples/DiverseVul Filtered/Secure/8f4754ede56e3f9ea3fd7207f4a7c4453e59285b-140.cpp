int bdrv_commit_all(void)
{
    BlockDriverState *bs;

    QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
        if (bs->drv && bs->backing_hd) {
            int ret = bdrv_commit(bs);
            if (ret < 0) {
                return ret;
            }
        }
    }
    return 0;
}