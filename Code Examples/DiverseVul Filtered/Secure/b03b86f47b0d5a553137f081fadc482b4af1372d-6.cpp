void connection_periodic_maint (server * const srv, const time_t cur_ts) {
    /* check all connections for timeouts */
    connections * const conns = &srv->conns;
    for (size_t ndx = 0; ndx < conns->used; ++ndx) {
        connection_check_timeout(conns->ptr[ndx], cur_ts);
    }
}