void mirror_start(const char *job_id, BlockDriverState *bs,
                  BlockDriverState *target, const char *replaces,
                  int creation_flags, int64_t speed,
                  uint32_t granularity, int64_t buf_size,
                  MirrorSyncMode mode, BlockMirrorBackingMode backing_mode,
                  bool zero_target,
                  BlockdevOnError on_source_error,
                  BlockdevOnError on_target_error,
                  bool unmap, const char *filter_node_name,
                  MirrorCopyMode copy_mode, Error **errp)
{
    bool is_none_mode;
    BlockDriverState *base;

    if ((mode == MIRROR_SYNC_MODE_INCREMENTAL) ||
        (mode == MIRROR_SYNC_MODE_BITMAP)) {
        error_setg(errp, "Sync mode '%s' not supported",
                   MirrorSyncMode_str(mode));
        return;
    }
    is_none_mode = mode == MIRROR_SYNC_MODE_NONE;
    base = mode == MIRROR_SYNC_MODE_TOP ? bdrv_backing_chain_next(bs) : NULL;
    mirror_start_job(job_id, bs, creation_flags, target, replaces,
                     speed, granularity, buf_size, backing_mode, zero_target,
                     on_source_error, on_target_error, unmap, NULL, NULL,
                     &mirror_job_driver, is_none_mode, base, false,
                     filter_node_name, true, copy_mode, errp);
}