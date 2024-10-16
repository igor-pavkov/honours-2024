int av_lockmgr_register(int (*cb)(void **mutex, enum AVLockOp op))
{
    if (lockmgr_cb) {
        if (lockmgr_cb(&codec_mutex, AV_LOCK_DESTROY))
            return -1;
        if (lockmgr_cb(&avformat_mutex, AV_LOCK_DESTROY))
            return -1;
    }

    lockmgr_cb = cb;

    if (lockmgr_cb) {
        if (lockmgr_cb(&codec_mutex, AV_LOCK_CREATE))
            return -1;
        if (lockmgr_cb(&avformat_mutex, AV_LOCK_CREATE))
            return -1;
    }
    return 0;
}