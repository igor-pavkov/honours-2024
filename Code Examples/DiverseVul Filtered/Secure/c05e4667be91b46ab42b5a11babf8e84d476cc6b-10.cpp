int qcow2_snapshot_goto(BlockDriverState *bs, const char *snapshot_id)
{
    BDRVQcowState *s = bs->opaque;
    QCowSnapshot *sn;
    int i, snapshot_index;
    int cur_l1_bytes, sn_l1_bytes;
    int ret;
    uint64_t *sn_l1_table = NULL;

    /* Search the snapshot */
    snapshot_index = find_snapshot_by_id_or_name(bs, snapshot_id);
    if (snapshot_index < 0) {
        return -ENOENT;
    }
    sn = &s->snapshots[snapshot_index];

    if (sn->disk_size != bs->total_sectors * BDRV_SECTOR_SIZE) {
        error_report("qcow2: Loading snapshots with different disk "
            "size is not implemented");
        ret = -ENOTSUP;
        goto fail;
    }

    /*
     * Make sure that the current L1 table is big enough to contain the whole
     * L1 table of the snapshot. If the snapshot L1 table is smaller, the
     * current one must be padded with zeros.
     */
    ret = qcow2_grow_l1_table(bs, sn->l1_size, true);
    if (ret < 0) {
        goto fail;
    }

    cur_l1_bytes = s->l1_size * sizeof(uint64_t);
    sn_l1_bytes = sn->l1_size * sizeof(uint64_t);

    /*
     * Copy the snapshot L1 table to the current L1 table.
     *
     * Before overwriting the old current L1 table on disk, make sure to
     * increase all refcounts for the clusters referenced by the new one.
     * Decrease the refcount referenced by the old one only when the L1
     * table is overwritten.
     */
    sn_l1_table = g_malloc0(cur_l1_bytes);

    ret = bdrv_pread(bs->file, sn->l1_table_offset, sn_l1_table, sn_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    ret = qcow2_update_snapshot_refcount(bs, sn->l1_table_offset,
                                         sn->l1_size, 1);
    if (ret < 0) {
        goto fail;
    }

    ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_ACTIVE_L1,
                                        s->l1_table_offset, cur_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    ret = bdrv_pwrite_sync(bs->file, s->l1_table_offset, sn_l1_table,
                           cur_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    /*
     * Decrease refcount of clusters of current L1 table.
     *
     * At this point, the in-memory s->l1_table points to the old L1 table,
     * whereas on disk we already have the new one.
     *
     * qcow2_update_snapshot_refcount special cases the current L1 table to use
     * the in-memory data instead of really using the offset to load a new one,
     * which is why this works.
     */
    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset,
                                         s->l1_size, -1);

    /*
     * Now update the in-memory L1 table to be in sync with the on-disk one. We
     * need to do this even if updating refcounts failed.
     */
    for(i = 0;i < s->l1_size; i++) {
        s->l1_table[i] = be64_to_cpu(sn_l1_table[i]);
    }

    if (ret < 0) {
        goto fail;
    }

    g_free(sn_l1_table);
    sn_l1_table = NULL;

    /*
     * Update QCOW_OFLAG_COPIED in the active L1 table (it may have changed
     * when we decreased the refcount of the old snapshot.
     */
    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset, s->l1_size, 0);
    if (ret < 0) {
        goto fail;
    }

#ifdef DEBUG_ALLOC
    {
        BdrvCheckResult result = {0};
        qcow2_check_refcounts(bs, &result, 0);
    }
#endif
    return 0;

fail:
    g_free(sn_l1_table);
    return ret;
}