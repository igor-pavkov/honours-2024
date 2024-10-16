void bdrv_close_all(void)
{
    BlockDriverState *bs;

    QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
        bdrv_close(bs);
    }
}