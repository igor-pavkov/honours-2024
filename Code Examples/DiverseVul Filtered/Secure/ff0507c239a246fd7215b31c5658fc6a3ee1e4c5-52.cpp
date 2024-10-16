static int coroutine_fn iscsi_co_copy_range_from(BlockDriverState *bs,
                                                 BdrvChild *src,
                                                 uint64_t src_offset,
                                                 BdrvChild *dst,
                                                 uint64_t dst_offset,
                                                 uint64_t bytes,
                                                 BdrvRequestFlags read_flags,
                                                 BdrvRequestFlags write_flags)
{
    return bdrv_co_copy_range_to(src, src_offset, dst, dst_offset, bytes,
                                 read_flags, write_flags);
}