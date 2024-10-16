static int coroutine_fn bdrv_co_do_preadv(BlockDriverState *bs,
    int64_t offset, unsigned int bytes, QEMUIOVector *qiov,
    BdrvRequestFlags flags)
{
    BlockDriver *drv = bs->drv;
    BdrvTrackedRequest req;

    /* TODO Lift BDRV_SECTOR_SIZE restriction in BlockDriver interface */
    uint64_t align = MAX(BDRV_SECTOR_SIZE, bs->request_alignment);
    uint8_t *head_buf = NULL;
    uint8_t *tail_buf = NULL;
    QEMUIOVector local_qiov;
    bool use_local_qiov = false;
    int ret;

    if (!drv) {
        return -ENOMEDIUM;
    }
    if (bdrv_check_byte_request(bs, offset, bytes)) {
        return -EIO;
    }

    if (bs->copy_on_read) {
        flags |= BDRV_REQ_COPY_ON_READ;
    }

    /* throttling disk I/O */
    if (bs->io_limits_enabled) {
        bdrv_io_limits_intercept(bs, bytes, false);
    }

    /* Align read if necessary by padding qiov */
    if (offset & (align - 1)) {
        head_buf = qemu_blockalign(bs, align);
        qemu_iovec_init(&local_qiov, qiov->niov + 2);
        qemu_iovec_add(&local_qiov, head_buf, offset & (align - 1));
        qemu_iovec_concat(&local_qiov, qiov, 0, qiov->size);
        use_local_qiov = true;

        bytes += offset & (align - 1);
        offset = offset & ~(align - 1);
    }

    if ((offset + bytes) & (align - 1)) {
        if (!use_local_qiov) {
            qemu_iovec_init(&local_qiov, qiov->niov + 1);
            qemu_iovec_concat(&local_qiov, qiov, 0, qiov->size);
            use_local_qiov = true;
        }
        tail_buf = qemu_blockalign(bs, align);
        qemu_iovec_add(&local_qiov, tail_buf,
                       align - ((offset + bytes) & (align - 1)));

        bytes = ROUND_UP(bytes, align);
    }

    tracked_request_begin(&req, bs, offset, bytes, false);
    ret = bdrv_aligned_preadv(bs, &req, offset, bytes, align,
                              use_local_qiov ? &local_qiov : qiov,
                              flags);
    tracked_request_end(&req);

    if (use_local_qiov) {
        qemu_iovec_destroy(&local_qiov);
        qemu_vfree(head_buf);
        qemu_vfree(tail_buf);
    }

    return ret;
}