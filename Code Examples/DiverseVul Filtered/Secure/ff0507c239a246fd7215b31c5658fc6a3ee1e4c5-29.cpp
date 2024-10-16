static int iscsi_reopen_prepare(BDRVReopenState *state,
                                BlockReopenQueue *queue, Error **errp)
{
    IscsiLun *iscsilun = state->bs->opaque;

    if (state->flags & BDRV_O_RDWR && iscsilun->write_protected) {
        error_setg(errp, "Cannot open a write protected LUN as read-write");
        return -EACCES;
    }
    return 0;
}