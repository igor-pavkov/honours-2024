size_t bdrv_opt_mem_align(BlockDriverState *bs)
{
    if (!bs || !bs->drv) {
        /* 4k should be on the safe side */
        return 4096;
    }

    return bs->bl.opt_mem_alignment;
}