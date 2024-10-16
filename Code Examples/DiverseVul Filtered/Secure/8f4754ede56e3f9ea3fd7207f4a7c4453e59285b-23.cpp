void bdrv_iostatus_reset(BlockDriverState *bs)
{
    if (bdrv_iostatus_is_enabled(bs)) {
        bs->iostatus = BLOCK_DEVICE_IO_STATUS_OK;
        if (bs->job) {
            block_job_iostatus_reset(bs->job);
        }
    }
}