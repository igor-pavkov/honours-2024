static chunk * connection_read_header_more(connection *con, chunkqueue *cq, chunk *c, const size_t olen) {
    /*(should not be reached by HTTP/2 streams)*/
    /*if (r->http_version == HTTP_VERSION_2) return NULL;*/
    /*(However, new connections over TLS may become HTTP/2 connections via ALPN
     * and return from this routine with r->http_version == HTTP_VERSION_2) */

    if ((NULL == c || NULL == c->next) && con->is_readable > 0) {
        con->read_idle_ts = log_epoch_secs;
        if (0 != con->network_read(con, cq, MAX_READ_LIMIT)) {
            request_st * const r = &con->request;
            connection_set_state_error(r, CON_STATE_ERROR);
        }
        /* check if switched to HTTP/2 (ALPN "h2" during TLS negotiation) */
        request_st * const r = &con->request;
        if (r->http_version == HTTP_VERSION_2) return NULL;
    }

    if (cq->first != cq->last && 0 != olen) {
        const size_t clen = chunkqueue_length(cq);
        size_t block = (olen + (16384-1)) & ~(16384-1);
        block += (block - olen > 1024 ? 0 : 16384);
        chunkqueue_compact_mem(cq, block > clen ? clen : block);
    }

    /* detect if data is added to chunk */
    c = cq->first;
    return (c && (size_t)c->offset + olen < buffer_string_length(c->mem))
      ? c
      : NULL;
}