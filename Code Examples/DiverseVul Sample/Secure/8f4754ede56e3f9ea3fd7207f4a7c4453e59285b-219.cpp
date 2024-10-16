void bdrv_set_guest_block_size(BlockDriverState *bs, int align)
{
    bs->guest_block_size = align;
}