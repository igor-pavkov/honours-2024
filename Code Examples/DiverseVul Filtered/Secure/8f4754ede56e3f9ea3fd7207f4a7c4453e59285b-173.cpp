int bdrv_has_zero_init(BlockDriverState *bs)
{
    assert(bs->drv);

    /* If BS is a copy on write image, it is initialized to
       the contents of the base image, which may not be zeroes.  */
    if (bs->backing_hd) {
        return 0;
    }
    if (bs->drv->bdrv_has_zero_init) {
        return bs->drv->bdrv_has_zero_init(bs);
    }

    /* safe default */
    return 0;
}