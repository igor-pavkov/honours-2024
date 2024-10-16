void bdrv_invalidate_cache_all(Error **errp)
{
    BlockDriverState *bs;
    Error *local_err = NULL;

    QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
        bdrv_invalidate_cache(bs, &local_err);
        if (local_err) {
            error_propagate(errp, local_err);
            return;
        }
    }
}