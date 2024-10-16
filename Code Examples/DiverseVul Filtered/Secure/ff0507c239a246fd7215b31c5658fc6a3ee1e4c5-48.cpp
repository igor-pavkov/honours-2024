static void iscsi_modesense_sync(IscsiLun *iscsilun)
{
    struct scsi_task *task;
    struct scsi_mode_sense *ms = NULL;
    iscsilun->write_protected = false;
    iscsilun->dpofua = false;

    task = iscsi_modesense6_sync(iscsilun->iscsi, iscsilun->lun,
                                 1, SCSI_MODESENSE_PC_CURRENT,
                                 0x3F, 0, 255);
    if (task == NULL) {
        error_report("iSCSI: Failed to send MODE_SENSE(6) command: %s",
                     iscsi_get_error(iscsilun->iscsi));
        goto out;
    }

    if (task->status != SCSI_STATUS_GOOD) {
        error_report("iSCSI: Failed MODE_SENSE(6), LUN assumed writable");
        goto out;
    }
    ms = scsi_datain_unmarshall(task);
    if (!ms) {
        error_report("iSCSI: Failed to unmarshall MODE_SENSE(6) data: %s",
                     iscsi_get_error(iscsilun->iscsi));
        goto out;
    }
    iscsilun->write_protected = ms->device_specific_parameter & 0x80;
    iscsilun->dpofua          = ms->device_specific_parameter & 0x10;

out:
    if (task) {
        scsi_free_scsi_task(task);
    }
}