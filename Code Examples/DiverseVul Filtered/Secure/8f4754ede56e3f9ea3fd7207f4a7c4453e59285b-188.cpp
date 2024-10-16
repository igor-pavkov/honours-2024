BlockDriverState *bdrv_find_base(BlockDriverState *bs)
{
    BlockDriverState *curr_bs = NULL;

    if (!bs) {
        return NULL;
    }

    curr_bs = bs;

    while (curr_bs->backing_hd) {
        curr_bs = curr_bs->backing_hd;
    }
    return curr_bs;
}