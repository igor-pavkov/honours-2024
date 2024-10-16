    OVS_REQUIRES(ctb->lock)
{
    ovs_assert(conn->conn_type == CT_CONN_TYPE_DEFAULT);

    if (conn->alg) {
        expectation_clean(ct, &conn->key, ct->hash_basis);
    }
    ovs_list_remove(&conn->exp_node);
    hmap_remove(&ctb->connections, &conn->node);
    atomic_count_dec(&ct->n_conn);
    if (conn->nat_info) {
        nat_clean(ct, conn, ctb);
    } else {
        delete_conn(conn);
    }
}