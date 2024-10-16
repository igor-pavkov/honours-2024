static struct scsi_task *iscsi_xcopy_task(int param_len)
{
    struct scsi_task *task;

    task = g_new0(struct scsi_task, 1);

    task->cdb[0]     = EXTENDED_COPY;
    task->cdb[10]    = (param_len >> 24) & 0xFF;
    task->cdb[11]    = (param_len >> 16) & 0xFF;
    task->cdb[12]    = (param_len >> 8) & 0xFF;
    task->cdb[13]    = param_len & 0xFF;
    task->cdb_size   = 16;
    task->xfer_dir   = SCSI_XFER_WRITE;
    task->expxferlen = param_len;

    return task;
}