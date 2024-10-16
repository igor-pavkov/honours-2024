static int handle_copied(BlockDriverState *bs, uint64_t guest_offset,
    uint64_t *host_offset, uint64_t *bytes, QCowL2Meta **m)
{
    BDRVQcowState *s = bs->opaque;
    int l2_index;
    uint64_t cluster_offset;
    uint64_t *l2_table;
    unsigned int nb_clusters;
    unsigned int keep_clusters;
    int ret, pret;

    trace_qcow2_handle_copied(qemu_coroutine_self(), guest_offset, *host_offset,
                              *bytes);

    assert(*host_offset == 0 ||    offset_into_cluster(s, guest_offset)
                                == offset_into_cluster(s, *host_offset));

    /*
     * Calculate the number of clusters to look for. We stop at L2 table
     * boundaries to keep things simple.
     */
    nb_clusters =
        size_to_clusters(s, offset_into_cluster(s, guest_offset) + *bytes);

    l2_index = offset_to_l2_index(s, guest_offset);
    nb_clusters = MIN(nb_clusters, s->l2_size - l2_index);

    /* Find L2 entry for the first involved cluster */
    ret = get_cluster_table(bs, guest_offset, &l2_table, &l2_index);
    if (ret < 0) {
        return ret;
    }

    cluster_offset = be64_to_cpu(l2_table[l2_index]);

    /* Check how many clusters are already allocated and don't need COW */
    if (qcow2_get_cluster_type(cluster_offset) == QCOW2_CLUSTER_NORMAL
        && (cluster_offset & QCOW_OFLAG_COPIED))
    {
        /* If a specific host_offset is required, check it */
        bool offset_matches =
            (cluster_offset & L2E_OFFSET_MASK) == *host_offset;

        if (*host_offset != 0 && !offset_matches) {
            *bytes = 0;
            ret = 0;
            goto out;
        }

        /* We keep all QCOW_OFLAG_COPIED clusters */
        keep_clusters =
            count_contiguous_clusters(nb_clusters, s->cluster_size,
                                      &l2_table[l2_index],
                                      QCOW_OFLAG_COPIED | QCOW_OFLAG_ZERO);
        assert(keep_clusters <= nb_clusters);

        *bytes = MIN(*bytes,
                 keep_clusters * s->cluster_size
                 - offset_into_cluster(s, guest_offset));

        ret = 1;
    } else {
        ret = 0;
    }

    /* Cleanup */
out:
    pret = qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);
    if (pret < 0) {
        return pret;
    }

    /* Only return a host offset if we actually made progress. Otherwise we
     * would make requirements for handle_alloc() that it can't fulfill */
    if (ret) {
        *host_offset = (cluster_offset & L2E_OFFSET_MASK)
                     + offset_into_cluster(s, guest_offset);
    }

    return ret;
}