static int iscsi_get_info(BlockDriverState *bs, BlockDriverInfo *bdi)
{
    IscsiLun *iscsilun = bs->opaque;
    bdi->unallocated_blocks_are_zero = iscsilun->lbprz;
    bdi->cluster_size = iscsilun->cluster_size;
    return 0;
}