void bdrv_ref(BlockDriverState *bs)
{
    bs->refcnt++;
}