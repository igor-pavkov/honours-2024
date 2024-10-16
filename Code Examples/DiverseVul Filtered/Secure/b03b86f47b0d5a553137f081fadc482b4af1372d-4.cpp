void connection_graceful_shutdown_maint (server *srv) {
    connections * const conns = &srv->conns;
    const int graceful_expire =
      (srv->graceful_expire_ts && srv->graceful_expire_ts < log_epoch_secs);
    for (uint32_t ndx = 0; ndx < conns->used; ++ndx) {
        connection * const con = conns->ptr[ndx];
        int changed = 0;

        request_st * const r = &con->request;
        if (r->state == CON_STATE_CLOSE) {
            /* reduce remaining linger timeout to be
             * (from zero) *up to* one more second, but no more */
            if (HTTP_LINGER_TIMEOUT > 1)
                con->close_timeout_ts -= (HTTP_LINGER_TIMEOUT - 1);
            if (log_epoch_secs - con->close_timeout_ts > HTTP_LINGER_TIMEOUT)
                changed = 1;
        }
        else if (con->h2 && r->state == CON_STATE_WRITE) {
            h2_send_goaway(con, H2_E_NO_ERROR);
            if (0 == con->h2->rused && chunkqueue_is_empty(con->write_queue)) {
                connection_set_state(r, CON_STATE_RESPONSE_END);
                changed = 1;
            }
        }
        else if (r->state == CON_STATE_READ && con->request_count > 1
                 && chunkqueue_is_empty(con->read_queue)) {
            /* close connections in keep-alive waiting for next request */
            connection_set_state_error(r, CON_STATE_ERROR);
            changed = 1;
        }

        if (graceful_expire) {
            connection_set_state_error(r, CON_STATE_ERROR);
            changed = 1;
        }

        r->keep_alive = 0;            /* disable keep-alive */

        r->conf.bytes_per_second = 0;         /* disable rate limit */
        r->conf.global_bytes_per_second = 0;  /* disable rate limit */
        if (con->traffic_limit_reached) {
            con->traffic_limit_reached = 0;
            changed = 1;
        }

        if (changed) {
            connection_state_machine(con);
        }
    }
}