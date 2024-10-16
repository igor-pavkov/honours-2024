void bdrv_invalidate_cache(BlockDriverState *bs, Error **errp)
{
    Error *local_err = NULL;
    int ret;

    if (!bs->drv)  {
        return;
    }

    if (bs->drv->bdrv_invalidate_cache) {
        bs->drv->bdrv_invalidate_cache(bs, &local_err);
    } else if (bs->file) {
        bdrv_invalidate_cache(bs->file, &local_err);
    }
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    ret = refresh_total_sectors(bs, bs->total_sectors);
    if (ret < 0) {
        error_setg_errno(errp, -ret, "Could not refresh total sector count");
        return;
    }
}