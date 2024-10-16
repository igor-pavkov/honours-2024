    OVS_REQUIRES(ctb->lock)
{
    struct conn *conn = NULL;

    HMAP_FOR_EACH_WITH_HASH (conn, node, hash, &ctb->connections) {
        if (!conn_key_cmp(&conn->key, key)
            && conn->conn_type == CT_CONN_TYPE_DEFAULT) {
            break;
        }
        if (!conn_key_cmp(&conn->rev_key, key)
            && conn->conn_type == CT_CONN_TYPE_DEFAULT) {
            break;
        }
    }
    return conn;
}