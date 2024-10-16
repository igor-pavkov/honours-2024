static int coroutine_fn iscsi_co_copy_range_to(BlockDriverState *bs,
                                               BdrvChild *src,
                                               uint64_t src_offset,
                                               BdrvChild *dst,
                                               uint64_t dst_offset,
                                               uint64_t bytes,
                                               BdrvRequestFlags read_flags,
                                               BdrvRequestFlags write_flags)
{
    IscsiLun *dst_lun = dst->bs->opaque;
    IscsiLun *src_lun;
    struct IscsiTask iscsi_task;
    struct iscsi_data data;
    int r = 0;
    int block_size;

    if (src->bs->drv->bdrv_co_copy_range_to != iscsi_co_copy_range_to) {
        return -ENOTSUP;
    }
    src_lun = src->bs->opaque;

    if (!src_lun->dd || !dst_lun->dd) {
        return -ENOTSUP;
    }
    if (!is_byte_request_lun_aligned(dst_offset, bytes, dst_lun)) {
        return -ENOTSUP;
    }
    if (!is_byte_request_lun_aligned(src_offset, bytes, src_lun)) {
        return -ENOTSUP;
    }
    if (dst_lun->block_size != src_lun->block_size ||
        !dst_lun->block_size) {
        return -ENOTSUP;
    }

    block_size = dst_lun->block_size;
    if (bytes / block_size > 65535) {
        return -ENOTSUP;
    }

    iscsi_xcopy_data(&data,
                     src_lun, src_offset / block_size,
                     dst_lun, dst_offset / block_size,
                     bytes / block_size);

    iscsi_co_init_iscsitask(dst_lun, &iscsi_task);

    qemu_mutex_lock(&dst_lun->mutex);
    iscsi_task.task = iscsi_xcopy_task(data.size);
retry:
    if (iscsi_scsi_command_async(dst_lun->iscsi, dst_lun->lun,
                                 iscsi_task.task, iscsi_co_generic_cb,
                                 &data,
                                 &iscsi_task) != 0) {
        r = -EIO;
        goto out_unlock;
    }

    iscsi_co_wait_for_task(&iscsi_task, dst_lun);

    if (iscsi_task.do_retry) {
        iscsi_task.complete = 0;
        goto retry;
    }

    if (iscsi_task.status != SCSI_STATUS_GOOD) {
        r = iscsi_task.err_code;
        goto out_unlock;
    }

out_unlock:

    trace_iscsi_xcopy(src_lun, src_offset, dst_lun, dst_offset, bytes, r);
    g_free(iscsi_task.task);
    qemu_mutex_unlock(&dst_lun->mutex);
    g_free(iscsi_task.err_str);
    return r;
}