static int coroutine_fn iscsi_co_block_status(BlockDriverState *bs,
                                              bool want_zero, int64_t offset,
                                              int64_t bytes, int64_t *pnum,
                                              int64_t *map,
                                              BlockDriverState **file)
{
    IscsiLun *iscsilun = bs->opaque;
    struct scsi_get_lba_status *lbas = NULL;
    struct scsi_lba_status_descriptor *lbasd = NULL;
    struct IscsiTask iTask;
    uint64_t lba, max_bytes;
    int ret;

    iscsi_co_init_iscsitask(iscsilun, &iTask);

    assert(QEMU_IS_ALIGNED(offset | bytes, iscsilun->block_size));

    /* default to all sectors allocated */
    ret = BDRV_BLOCK_DATA | BDRV_BLOCK_OFFSET_VALID;
    if (map) {
        *map = offset;
    }
    *pnum = bytes;

    /* LUN does not support logical block provisioning */
    if (!iscsilun->lbpme) {
        goto out;
    }

    lba = offset / iscsilun->block_size;
    max_bytes = (iscsilun->num_blocks - lba) * iscsilun->block_size;

    qemu_mutex_lock(&iscsilun->mutex);
retry:
    if (iscsi_get_lba_status_task(iscsilun->iscsi, iscsilun->lun,
                                  lba, 8 + 16, iscsi_co_generic_cb,
                                  &iTask) == NULL) {
        ret = -ENOMEM;
        goto out_unlock;
    }
    iscsi_co_wait_for_task(&iTask, iscsilun);

    if (iTask.do_retry) {
        if (iTask.task != NULL) {
            scsi_free_scsi_task(iTask.task);
            iTask.task = NULL;
        }
        iTask.complete = 0;
        goto retry;
    }

    if (iTask.status != SCSI_STATUS_GOOD) {
        /* in case the get_lba_status_callout fails (i.e.
         * because the device is busy or the cmd is not
         * supported) we pretend all blocks are allocated
         * for backwards compatibility */
        error_report("iSCSI GET_LBA_STATUS failed at lba %" PRIu64 ": %s",
                     lba, iTask.err_str);
        goto out_unlock;
    }

    lbas = scsi_datain_unmarshall(iTask.task);
    if (lbas == NULL || lbas->num_descriptors == 0) {
        ret = -EIO;
        goto out_unlock;
    }

    lbasd = &lbas->descriptors[0];

    if (lba != lbasd->lba) {
        ret = -EIO;
        goto out_unlock;
    }

    *pnum = MIN((int64_t) lbasd->num_blocks * iscsilun->block_size, max_bytes);

    if (lbasd->provisioning == SCSI_PROVISIONING_TYPE_DEALLOCATED ||
        lbasd->provisioning == SCSI_PROVISIONING_TYPE_ANCHORED) {
        ret &= ~BDRV_BLOCK_DATA;
        if (iscsilun->lbprz) {
            ret |= BDRV_BLOCK_ZERO;
        }
    }

    if (ret & BDRV_BLOCK_ZERO) {
        iscsi_allocmap_set_unallocated(iscsilun, offset, *pnum);
    } else {
        iscsi_allocmap_set_allocated(iscsilun, offset, *pnum);
    }

    if (*pnum > bytes) {
        *pnum = bytes;
    }
out_unlock:
    qemu_mutex_unlock(&iscsilun->mutex);
    g_free(iTask.err_str);
out:
    if (iTask.task != NULL) {
        scsi_free_scsi_task(iTask.task);
    }
    if (ret > 0 && ret & BDRV_BLOCK_OFFSET_VALID && file) {
        *file = bs;
    }
    return ret;
}