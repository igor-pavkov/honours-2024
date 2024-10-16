static int discard_single_l2(BlockDriverState *bs, uint64_t offset,
    unsigned int nb_clusters, enum qcow2_discard_type type)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t *l2_table;
    int l2_index;
    int ret;
    int i;

    ret = get_cluster_table(bs, offset, &l2_table, &l2_index);
    if (ret < 0) {
        return ret;
    }

    /* Limit nb_clusters to one L2 table */
    nb_clusters = MIN(nb_clusters, s->l2_size - l2_index);

    for (i = 0; i < nb_clusters; i++) {
        uint64_t old_offset;

        old_offset = be64_to_cpu(l2_table[l2_index + i]);

        /*
         * Make sure that a discarded area reads back as zeroes for v3 images
         * (we cannot do it for v2 without actually writing a zero-filled
         * buffer). We can skip the operation if the cluster is already marked
         * as zero, or if it's unallocated and we don't have a backing file.
         *
         * TODO We might want to use bdrv_get_block_status(bs) here, but we're
         * holding s->lock, so that doesn't work today.
         */
        if (old_offset & QCOW_OFLAG_ZERO) {
            continue;
        }

        if ((old_offset & L2E_OFFSET_MASK) == 0 && !bs->backing_hd) {
            continue;
        }

        /* First remove L2 entries */
        qcow2_cache_entry_mark_dirty(s->l2_table_cache, l2_table);
        if (s->qcow_version >= 3) {
            l2_table[l2_index + i] = cpu_to_be64(QCOW_OFLAG_ZERO);
        } else {
            l2_table[l2_index + i] = cpu_to_be64(0);
        }

        /* Then decrease the refcount */
        qcow2_free_any_clusters(bs, old_offset, 1, type);
    }

    ret = qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);
    if (ret < 0) {
        return ret;
    }

    return nb_clusters;
}