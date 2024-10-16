static int iscsi_allocmap_init(IscsiLun *iscsilun, int open_flags)
{
    iscsi_allocmap_free(iscsilun);

    assert(iscsilun->cluster_size);
    iscsilun->allocmap_size =
        DIV_ROUND_UP(iscsilun->num_blocks * iscsilun->block_size,
                     iscsilun->cluster_size);

    iscsilun->allocmap = bitmap_try_new(iscsilun->allocmap_size);
    if (!iscsilun->allocmap) {
        return -ENOMEM;
    }

    if (open_flags & BDRV_O_NOCACHE) {
        /* when cache.direct = on all allocmap entries are
         * treated as invalid to force a relookup of the block
         * status on every read request */
        return 0;
    }

    iscsilun->allocmap_valid = bitmap_try_new(iscsilun->allocmap_size);
    if (!iscsilun->allocmap_valid) {
        /* if we are under memory pressure free the allocmap as well */
        iscsi_allocmap_free(iscsilun);
        return -ENOMEM;
    }

    return 0;
}