static int coroutine_fn iscsi_co_flush(BlockDriverState *bs)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    int r = 0;

    iscsi_co_init_iscsitask(iscsilun, &iTask);
    qemu_mutex_lock(&iscsilun->mutex);
retry:
    if (iscsi_synchronizecache10_task(iscsilun->iscsi, iscsilun->lun, 0, 0, 0,
                                      0, iscsi_co_generic_cb, &iTask) == NULL) {
        qemu_mutex_unlock(&iscsilun->mutex);
        return -ENOMEM;
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

    if (iTask.status != SCSI_STATUS_GOOD) {
        error_report("iSCSI SYNCHRONIZECACHE10 failed: %s", iTask.err_str);
        r = iTask.err_code;
    }

    qemu_mutex_unlock(&iscsilun->mutex);
    g_free(iTask.err_str);
    return r;
}