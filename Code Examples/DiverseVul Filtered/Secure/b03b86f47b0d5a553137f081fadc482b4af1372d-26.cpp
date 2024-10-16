static void connection_check_timeout (connection * const con, const time_t cur_ts) {
    const int waitevents = fdevent_fdnode_interest(con->fdn);
    int changed = 0;
    int t_diff;

    request_st * const r = &con->request;
    if (r->state == CON_STATE_CLOSE) {
        if (cur_ts - con->close_timeout_ts > HTTP_LINGER_TIMEOUT) {
            changed = 1;
        }
    }
    else if (con->h2 && r->state == CON_STATE_WRITE) {
        h2con * const h2c = con->h2;
        if (h2c->rused) {
            for (uint32_t i = 0; i < h2c->rused; ++i) {
                request_st * const rr = h2c->r[i];
                if (rr->state == CON_STATE_ERROR) { /*(should not happen)*/
                    changed = 1;
                    continue;
                }
                if (rr->reqbody_length != rr->reqbody_queue.bytes_in) {
                    /* XXX: should timeout apply if not trying to read on h2con?
                     * (still applying timeout to catch stuck connections) */
                    /* XXX: con->read_idle_ts is not per-request, so timeout
                     * will not occur if other read activity occurs on h2con
                     * (future: might keep separate timestamp per-request) */
                    if (cur_ts - con->read_idle_ts > rr->conf.max_read_idle) {
                        /* time - out */
                        if (rr->conf.log_request_handling) {
                            log_error(rr->conf.errh, __FILE__, __LINE__,
                              "request aborted - read timeout: %d", con->fd);
                        }
                        connection_set_state_error(r, CON_STATE_ERROR);
                        changed = 1;
                    }
                }

                if (rr->state != CON_STATE_READ_POST
                    && con->write_request_ts != 0) {
                    /* XXX: con->write_request_ts is not per-request, so timeout
                     * will not occur if other write activity occurs on h2con
                     * (future: might keep separate timestamp per-request) */
                    if (cur_ts - con->write_request_ts
                        > r->conf.max_write_idle) {
                        /*(see comment further down about max_write_idle)*/
                        /* time - out */
                        if (r->conf.log_timeouts) {
                            log_error(r->conf.errh, __FILE__, __LINE__,
                              "NOTE: a request from %.*s for %.*s timed out "
                              "after writing %lld bytes. We waited %d seconds. "
                              "If this is a problem, increase "
                              "server.max-write-idle",
                              BUFFER_INTLEN_PTR(con->dst_addr_buf),
                              BUFFER_INTLEN_PTR(&r->target),
                              (long long)r->write_queue.bytes_out,
                              (int)r->conf.max_write_idle);
                        }
                        connection_set_state_error(r, CON_STATE_ERROR);
                        changed = 1;
                    }
                }
            }
        }
        else {
            if (cur_ts - con->read_idle_ts > con->keep_alive_idle) {
                /* time - out */
                if (r->conf.log_request_handling) {
                    log_error(r->conf.errh, __FILE__, __LINE__,
                              "connection closed - keep-alive timeout: %d",
                              con->fd);
                }
                connection_set_state(r, CON_STATE_RESPONSE_END);
                changed = 1;
            }
        }
    }
    else if (waitevents & FDEVENT_IN) {
        if (con->request_count == 1 || r->state != CON_STATE_READ) {
            /* e.g. CON_STATE_READ_POST || CON_STATE_WRITE */
            if (cur_ts - con->read_idle_ts > r->conf.max_read_idle) {
                /* time - out */
                if (r->conf.log_request_handling) {
                    log_error(r->conf.errh, __FILE__, __LINE__,
                              "connection closed - read timeout: %d", con->fd);
                }

                connection_set_state_error(r, CON_STATE_ERROR);
                changed = 1;
            }
        } else {
            if (cur_ts - con->read_idle_ts > con->keep_alive_idle) {
                /* time - out */
                if (r->conf.log_request_handling) {
                    log_error(r->conf.errh, __FILE__, __LINE__,
                              "connection closed - keep-alive timeout: %d",
                              con->fd);
                }

                connection_set_state_error(r, CON_STATE_ERROR);
                changed = 1;
            }
        }
    }

    /* max_write_idle timeout currently functions as backend timeout,
     * too, after response has been started.
     * future: have separate backend timeout, and then change this
     * to check for write interest before checking for timeout */
    /*if (waitevents & FDEVENT_OUT)*/
    if (r->http_version <= HTTP_VERSION_1_1
        && r->state == CON_STATE_WRITE && con->write_request_ts != 0) {
      #if 0
        if (cur_ts - con->write_request_ts > 60) {
            log_error(r->conf.errh, __FILE__, __LINE__,
                      "connection closed - pre-write-request-timeout: %d %d",
                      con->fd, cur_ts - con->write_request_ts);
        }
      #endif

        if (cur_ts - con->write_request_ts > r->conf.max_write_idle) {
            /* time - out */
            if (r->conf.log_timeouts) {
                log_error(r->conf.errh, __FILE__, __LINE__,
                  "NOTE: a request from %.*s for %.*s timed out after writing "
                  "%lld bytes. We waited %d seconds.  If this is a problem, "
                  "increase server.max-write-idle",
                  BUFFER_INTLEN_PTR(con->dst_addr_buf),
                  BUFFER_INTLEN_PTR(&r->target),
                  (long long)con->bytes_written, (int)r->conf.max_write_idle);
            }
            connection_set_state_error(r, CON_STATE_ERROR);
            changed = 1;
        }
    }

    /* lighttpd HTTP/2 limitation: rate limit config r->conf.bytes_per_second
     * (currently) taken only from top-level config (socket), with host if SNI
     * used, but not any other config conditions, e.g. not per-file-type */

    if (0 == (t_diff = cur_ts - con->connection_start)) t_diff = 1;

    if (con->traffic_limit_reached &&
        (r->conf.bytes_per_second == 0 ||
         con->bytes_written < (off_t)r->conf.bytes_per_second * t_diff)) {
        /* enable connection again */
        con->traffic_limit_reached = 0;

        changed = 1;
    }

    con->bytes_written_cur_second = 0;

    if (changed) {
        connection_state_machine(con);
    }
}