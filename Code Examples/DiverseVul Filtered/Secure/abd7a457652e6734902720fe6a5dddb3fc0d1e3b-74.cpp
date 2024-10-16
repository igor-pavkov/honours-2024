    OVS_REQUIRES(ctb->lock)
{
    struct conn *conn, *next;
    long long min_expiration = LLONG_MAX;
    size_t count = 0;

    for (unsigned i = 0; i < N_CT_TM; i++) {
        LIST_FOR_EACH_SAFE (conn, next, exp_node, &ctb->exp_lists[i]) {
            if (!conn_expired(conn, now) || count >= limit) {
                min_expiration = MIN(min_expiration, conn->expiration);
                if (count >= limit) {
                    /* Do not check other lists. */
                    COVERAGE_INC(conntrack_long_cleanup);
                    return min_expiration;
                }
                break;
            }
            conn_clean(ct, conn, ctb);
            count++;
        }
    }
    return min_expiration;
}