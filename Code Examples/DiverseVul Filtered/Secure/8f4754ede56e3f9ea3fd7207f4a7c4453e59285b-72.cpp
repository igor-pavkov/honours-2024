int bdrv_reopen_prepare(BDRVReopenState *reopen_state, BlockReopenQueue *queue,
                        Error **errp)
{
    int ret = -1;
    Error *local_err = NULL;
    BlockDriver *drv;

    assert(reopen_state != NULL);
    assert(reopen_state->bs->drv != NULL);
    drv = reopen_state->bs->drv;

    /* if we are to stay read-only, do not allow permission change
     * to r/w */
    if (!(reopen_state->bs->open_flags & BDRV_O_ALLOW_RDWR) &&
        reopen_state->flags & BDRV_O_RDWR) {
        error_set(errp, QERR_DEVICE_IS_READ_ONLY,
                  reopen_state->bs->device_name);
        goto error;
    }


    ret = bdrv_flush(reopen_state->bs);
    if (ret) {
        error_set(errp, ERROR_CLASS_GENERIC_ERROR, "Error (%s) flushing drive",
                  strerror(-ret));
        goto error;
    }

    if (drv->bdrv_reopen_prepare) {
        ret = drv->bdrv_reopen_prepare(reopen_state, queue, &local_err);
        if (ret) {
            if (local_err != NULL) {
                error_propagate(errp, local_err);
            } else {
                error_setg(errp, "failed while preparing to reopen image '%s'",
                           reopen_state->bs->filename);
            }
            goto error;
        }
    } else {
        /* It is currently mandatory to have a bdrv_reopen_prepare()
         * handler for each supported drv. */
        error_set(errp, QERR_BLOCK_FORMAT_FEATURE_NOT_SUPPORTED,
                  drv->format_name, reopen_state->bs->device_name,
                 "reopening of file");
        ret = -1;
        goto error;
    }

    ret = 0;

error:
    return ret;
}