void bdrv_append(BlockDriverState *bs_new, BlockDriverState *bs_top)
{
    bdrv_swap(bs_new, bs_top);

    /* The contents of 'tmp' will become bs_top, as we are
     * swapping bs_new and bs_top contents. */
    bs_top->backing_hd = bs_new;
    bs_top->open_flags &= ~BDRV_O_NO_BACKING;
    pstrcpy(bs_top->backing_file, sizeof(bs_top->backing_file),
            bs_new->filename);
    pstrcpy(bs_top->backing_format, sizeof(bs_top->backing_format),
            bs_new->drv ? bs_new->drv->format_name : "");
}