static int qcow2_get_info(BlockDriverState *bs, BlockDriverInfo *bdi)
{
    BDRVQcowState *s = bs->opaque;
    bdi->unallocated_blocks_are_zero = true;
    bdi->can_write_zeroes_with_unmap = (s->qcow_version >= 3);
    bdi->cluster_size = s->cluster_size;
    bdi->vm_state_offset = qcow2_vm_state_offset(s);
    return 0;
}