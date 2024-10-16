coroutine_fn iscsi_co_pwrite_zeroes(BlockDriverState *bs, int64_t offset,
                                    int bytes, BdrvRequestFlags flags)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    uint64_t lba;
    uint32_t nb_blocks;
    bool use_16_for_ws = iscsilun->use_16_for_rw;
    int r = 0;

    if (!is_byte_request_lun_aligned(offset, bytes, iscsilun)) {
        return -ENOTSUP;
    }

    if (flags & BDRV_REQ_MAY_UNMAP) {
        if (!use_16_for_ws && !iscsilun->lbp.lbpws10) {
            /* WRITESAME10 with UNMAP is unsupported try WRITESAME16 */
            use_16_for_ws = true;
        }
        if (use_16_for_ws && !iscsilun->lbp.lbpws) {
            /* WRITESAME16 with UNMAP is not supported by the target,
             * fall back and try WRITESAME10/16 without UNMAP */
            flags &= ~BDRV_REQ_MAY_UNMAP;
            use_16_for_ws = iscsilun->use_16_for_rw;
        }
    }

    if (!(flags & BDRV_REQ_MAY_UNMAP) && !iscsilun->has_write_same) {
        /* WRITESAME without UNMAP is not supported by the target */
        return -ENOTSUP;
    }

    lba = offset / iscsilun->block_size;
    nb_blocks = bytes / iscsilun->block_size;

    if (iscsilun->zeroblock == NULL) {
        iscsilun->zeroblock = g_try_malloc0(iscsilun->block_size);
        if (iscsilun->zeroblock == NULL) {
            return -ENOMEM;
        }
    }

    qemu_mutex_lock(&iscsilun->mutex);
    iscsi_co_init_iscsitask(iscsilun, &iTask);
retry:
    if (use_16_for_ws) {
        iTask.task = iscsi_writesame16_task(iscsilun->iscsi, iscsilun->lun, lba,
                                            iscsilun->zeroblock, iscsilun->block_size,
                                            nb_blocks, 0, !!(flags & BDRV_REQ_MAY_UNMAP),
                                            0, 0, iscsi_co_generic_cb, &iTask);
    } else {
        iTask.task = iscsi_writesame10_task(iscsilun->iscsi, iscsilun->lun, lba,
                                            iscsilun->zeroblock, iscsilun->block_size,
                                            nb_blocks, 0, !!(flags & BDRV_REQ_MAY_UNMAP),
                                            0, 0, iscsi_co_generic_cb, &iTask);
    }
    if (iTask.task == NULL) {
        qemu_mutex_unlock(&iscsilun->mutex);
        return -ENOMEM;
    }

    iscsi_co_wait_for_task(&iTask, iscsilun);

    if (iTask.status == SCSI_STATUS_CHECK_CONDITION &&
        iTask.task->sense.key == SCSI_SENSE_ILLEGAL_REQUEST &&
        (iTask.task->sense.ascq == SCSI_SENSE_ASCQ_INVALID_OPERATION_CODE ||
         iTask.task->sense.ascq == SCSI_SENSE_ASCQ_INVALID_FIELD_IN_CDB)) {
        /* WRITE SAME is not supported by the target */
        iscsilun->has_write_same = false;
        scsi_free_scsi_task(iTask.task);
        r = -ENOTSUP;
        goto out_unlock;
    }

    if (iTask.task != NULL) {
        scsi_free_scsi_task(iTask.task);
        iTask.task = NULL;
    }

    if (iTask.do_retry) {
        iTask.complete = 0;
        goto retry;
    }

    if (iTask.status != SCSI_STATUS_GOOD) {
        iscsi_allocmap_set_invalid(iscsilun, offset, bytes);
        error_report("iSCSI WRITESAME10/16 failed at lba %" PRIu64 ": %s",
                     lba, iTask.err_str);
        r = iTask.err_code;
        goto out_unlock;
    }

    if (flags & BDRV_REQ_MAY_UNMAP) {
        iscsi_allocmap_set_invalid(iscsilun, offset, bytes);
    } else {
        iscsi_allocmap_set_allocated(iscsilun, offset, bytes);
    }

out_unlock:
    qemu_mutex_unlock(&iscsilun->mutex);
    g_free(iTask.err_str);
    return r;
}