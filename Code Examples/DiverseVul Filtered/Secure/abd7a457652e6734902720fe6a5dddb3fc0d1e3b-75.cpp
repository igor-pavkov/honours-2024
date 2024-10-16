    OVS_REQUIRES(ctb->lock)
{
    uint32_t hash = ctx->hash;
    struct conn *conn;

    ctx->conn = NULL;

    HMAP_FOR_EACH_WITH_HASH (conn, node, hash, &ctb->connections) {
        if (!conn_key_cmp(&conn->key, &ctx->key)
                && !conn_expired(conn, now)) {
            ctx->conn = conn;
            ctx->reply = false;
            break;
        }
        if (!conn_key_cmp(&conn->rev_key, &ctx->key)
                && !conn_expired(conn, now)) {
            ctx->conn = conn;
            ctx->reply = true;
            break;
        }
    }
}