void bdrv_unref(BlockDriverState *bs)
{
    assert(bs->refcnt > 0);
    if (--bs->refcnt == 0) {
        bdrv_delete(bs);
    }
}