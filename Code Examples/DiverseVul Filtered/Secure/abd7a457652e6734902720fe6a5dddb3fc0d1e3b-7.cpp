    OVS_REQUIRES(ctb->lock)
{
    ct_rwlock_wrlock(&ct->resources_lock);
    nat_conn_keys_remove(&ct->nat_conn_keys, &conn->rev_key, ct->hash_basis);
    ct_rwlock_unlock(&ct->resources_lock);
    ct_lock_unlock(&ctb->lock);
    uint32_t hash = conn_key_hash(&conn->rev_key, ct->hash_basis);
    unsigned bucket_rev_conn = hash_to_bucket(hash);
    ct_lock_lock(&ct->buckets[bucket_rev_conn].lock);
    ct_rwlock_wrlock(&ct->resources_lock);
    struct conn *rev_conn = conn_lookup_unnat(&conn->rev_key,
                                              &ct->buckets[bucket_rev_conn],
                                              hash);
    struct nat_conn_key_node *nat_conn_key_node =
        nat_conn_keys_lookup(&ct->nat_conn_keys, &conn->rev_key,
                             ct->hash_basis);

    /* In the unlikely event, rev conn was recreated, then skip
     * rev_conn cleanup. */
    if (rev_conn && (!nat_conn_key_node ||
                     conn_key_cmp(&nat_conn_key_node->value,
                                  &rev_conn->rev_key))) {
        hmap_remove(&ct->buckets[bucket_rev_conn].connections,
                    &rev_conn->node);
        free(rev_conn);
    }

    delete_conn(conn);
    ct_rwlock_unlock(&ct->resources_lock);
    ct_lock_unlock(&ct->buckets[bucket_rev_conn].lock);
    ct_lock_lock(&ctb->lock);
}