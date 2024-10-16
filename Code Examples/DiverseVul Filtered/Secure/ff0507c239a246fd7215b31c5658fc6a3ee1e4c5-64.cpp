coroutine_fn iscsi_co_pdiscard(BlockDriverState *bs, int64_t offset, int bytes)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    struct unmap_list list;
    int r = 0;

    if (!is_byte_request_lun_aligned(offset, bytes, iscsilun)) {
        return -ENOTSUP;
    }

    if (!iscsilun->lbp.lbpu) {
        /* UNMAP is not supported by the target */
        return 0;
    }

    list.lba = offset / iscsilun->block_size;
    list.num = bytes / iscsilun->block_size;

    iscsi_co_init_iscsitask(iscsilun, &iTask);
    qemu_mutex_lock(&iscsilun->mutex);
retry:
    if (iscsi_unmap_task(iscsilun->iscsi, iscsilun->lun, 0, 0, &list, 1,
                         iscsi_co_generic_cb, &iTask) == NULL) {
        r = -ENOMEM;
        goto out_unlock;
    }

    iscsi_co_wait_for_task(&iTask, iscsilun);

    if (iTask.task != NULL) {
        scsi_free_scsi_task(iTask.task);
        iTask.task = NULL;
    }

    if (iTask.do_retry) {
        iTask.complete = 0;
        goto retry;
    }

    iscsi_allocmap_set_invalid(iscsilun, offset, bytes);

    if (iTask.status == SCSI_STATUS_CHECK_CONDITION) {
        /* the target might fail with a check condition if it
           is not happy with the alignment of the UNMAP request
           we silently fail in this case */
        goto out_unlock;
    }

    if (iTask.status != SCSI_STATUS_GOOD) {
        error_report("iSCSI UNMAP failed at lba %" PRIu64 ": %s",
                     list.lba, iTask.err_str);
        r = iTask.err_code;
        goto out_unlock;
    }

out_unlock:
    qemu_mutex_unlock(&iscsilun->mutex);
    g_free(iTask.err_str);
    return r;
}