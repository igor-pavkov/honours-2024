static int coroutine_fn iscsi_co_truncate(BlockDriverState *bs, int64_t offset,
                                          bool exact, PreallocMode prealloc,
                                          Error **errp)
{
    IscsiLun *iscsilun = bs->opaque;
    int64_t cur_length;
    Error *local_err = NULL;

    if (prealloc != PREALLOC_MODE_OFF) {
        error_setg(errp, "Unsupported preallocation mode '%s'",
                   PreallocMode_str(prealloc));
        return -ENOTSUP;
    }

    if (iscsilun->type != TYPE_DISK) {
        error_setg(errp, "Cannot resize non-disk iSCSI devices");
        return -ENOTSUP;
    }

    iscsi_readcapacity_sync(iscsilun, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return -EIO;
    }

    cur_length = iscsi_getlength(bs);
    if (offset != cur_length && exact) {
        error_setg(errp, "Cannot resize iSCSI devices");
        return -ENOTSUP;
    } else if (offset > cur_length) {
        error_setg(errp, "Cannot grow iSCSI devices");
        return -EINVAL;
    }

    if (iscsilun->allocmap != NULL) {
        iscsi_allocmap_init(iscsilun, bs->open_flags);
    }

    return 0;
}