void bdrv_iostatus_set_err(BlockDriverState *bs, int error)
{
    assert(bdrv_iostatus_is_enabled(bs));
    if (bs->iostatus == BLOCK_DEVICE_IO_STATUS_OK) {
        bs->iostatus = error == ENOSPC ? BLOCK_DEVICE_IO_STATUS_NOSPACE :
                                         BLOCK_DEVICE_IO_STATUS_FAILED;
    }
}