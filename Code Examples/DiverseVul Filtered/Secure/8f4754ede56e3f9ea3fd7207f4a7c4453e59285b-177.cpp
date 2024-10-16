int bdrv_amend_options(BlockDriverState *bs, QEMUOptionParameter *options)
{
    if (bs->drv->bdrv_amend_options == NULL) {
        return -ENOTSUP;
    }
    return bs->drv->bdrv_amend_options(bs, options);
}