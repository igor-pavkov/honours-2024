int bdrv_refresh_limits(BlockDriverState *bs)
{
    BlockDriver *drv = bs->drv;

    memset(&bs->bl, 0, sizeof(bs->bl));

    if (!drv) {
        return 0;
    }

    /* Take some limits from the children as a default */
    if (bs->file) {
        bdrv_refresh_limits(bs->file);
        bs->bl.opt_transfer_length = bs->file->bl.opt_transfer_length;
        bs->bl.opt_mem_alignment = bs->file->bl.opt_mem_alignment;
    } else {
        bs->bl.opt_mem_alignment = 512;
    }

    if (bs->backing_hd) {
        bdrv_refresh_limits(bs->backing_hd);
        bs->bl.opt_transfer_length =
            MAX(bs->bl.opt_transfer_length,
                bs->backing_hd->bl.opt_transfer_length);
        bs->bl.opt_mem_alignment =
            MAX(bs->bl.opt_mem_alignment,
                bs->backing_hd->bl.opt_mem_alignment);
    }

    /* Then let the driver override it */
    if (drv->bdrv_refresh_limits) {
        return drv->bdrv_refresh_limits(bs);
    }

    return 0;
}