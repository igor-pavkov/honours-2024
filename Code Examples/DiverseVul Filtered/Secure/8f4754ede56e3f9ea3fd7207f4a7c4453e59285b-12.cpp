void *bdrv_get_attached_dev(BlockDriverState *bs)
{
    return bs->dev;
}