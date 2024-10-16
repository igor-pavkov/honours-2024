static void bdrv_rebind(BlockDriverState *bs)
{
    if (bs->drv && bs->drv->bdrv_rebind) {
        bs->drv->bdrv_rebind(bs);
    }
}