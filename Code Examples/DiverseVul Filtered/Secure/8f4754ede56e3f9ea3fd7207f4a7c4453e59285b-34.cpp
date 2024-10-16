BlockErrorAction bdrv_get_error_action(BlockDriverState *bs, bool is_read, int error)
{
    BlockdevOnError on_err = is_read ? bs->on_read_error : bs->on_write_error;

    switch (on_err) {
    case BLOCKDEV_ON_ERROR_ENOSPC:
        return (error == ENOSPC) ? BDRV_ACTION_STOP : BDRV_ACTION_REPORT;
    case BLOCKDEV_ON_ERROR_STOP:
        return BDRV_ACTION_STOP;
    case BLOCKDEV_ON_ERROR_REPORT:
        return BDRV_ACTION_REPORT;
    case BLOCKDEV_ON_ERROR_IGNORE:
        return BDRV_ACTION_IGNORE;
    default:
        abort();
    }
}