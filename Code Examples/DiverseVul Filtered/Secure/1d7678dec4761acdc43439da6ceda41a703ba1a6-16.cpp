static int vhdx_get_info(BlockDriverState *bs, BlockDriverInfo *bdi)
{
    BDRVVHDXState *s = bs->opaque;

    bdi->cluster_size = s->block_size;

    bdi->unallocated_blocks_are_zero =
        (s->params.data_bits & VHDX_PARAMS_HAS_PARENT) == 0;

    return 0;
}