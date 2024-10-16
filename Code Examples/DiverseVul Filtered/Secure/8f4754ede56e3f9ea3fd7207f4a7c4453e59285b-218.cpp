bool bdrv_iostatus_is_enabled(const BlockDriverState *bs)
{
    return (bs->iostatus_enabled &&
           (bs->on_write_error == BLOCKDEV_ON_ERROR_ENOSPC ||
            bs->on_write_error == BLOCKDEV_ON_ERROR_STOP   ||
            bs->on_read_error == BLOCKDEV_ON_ERROR_STOP));
}