BlockJob *commit_active_start(const char *job_id, BlockDriverState *bs,
                              BlockDriverState *base, int creation_flags,
                              int64_t speed, BlockdevOnError on_error,
                              const char *filter_node_name,
                              BlockCompletionFunc *cb, void *opaque,
                              bool auto_complete, Error **errp)
{
    bool base_read_only;
    BlockJob *job;

    base_read_only = bdrv_is_read_only(base);

    if (base_read_only) {
        if (bdrv_reopen_set_read_only(base, false, errp) < 0) {
            return NULL;
        }
    }

    job = mirror_start_job(
                     job_id, bs, creation_flags, base, NULL, speed, 0, 0,
                     MIRROR_LEAVE_BACKING_CHAIN, false,
                     on_error, on_error, true, cb, opaque,
                     &commit_active_job_driver, false, base, auto_complete,
                     filter_node_name, false, MIRROR_COPY_MODE_BACKGROUND,
                     errp);
    if (!job) {
        goto error_restore_flags;
    }

    return job;

error_restore_flags:
    /* ignore error and errp for bdrv_reopen, because we want to propagate
     * the original error */
    if (base_read_only) {
        bdrv_reopen_set_read_only(base, true, NULL);
    }
    return NULL;
}