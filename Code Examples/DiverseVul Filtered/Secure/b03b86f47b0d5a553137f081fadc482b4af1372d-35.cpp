static int connection_handle_read_state(connection * const con)  {
    /*(should not be reached by HTTP/2 streams)*/
    chunkqueue * const cq = con->read_queue;
    chunk *c = cq->first;
    uint32_t clen = 0;
    uint32_t header_len = 0;
    request_st * const r = &con->request;
    uint8_t keepalive_request_start = 0;
    uint8_t pipelined_request_start = 0;
    uint8_t discard_blank = 0;
    unsigned short hoff[8192]; /* max num header lines + 3; 16k on stack */

    if (con->request_count > 1) {
        discard_blank = 1;
        if (con->bytes_read == r->bytes_read_ckpt) {
            keepalive_request_start = 1;
            if (NULL != c) { /* !chunkqueue_is_empty(cq)) */
                pipelined_request_start = 1;
                /* partial header of next request has already been read,
                 * so optimistically check for more data received on
                 * socket while processing the previous request */
                con->is_readable = 1;
                /*(if partially read next request and unable to read any bytes,
                 * then will unnecessarily scan again before subsequent read)*/
            }
        }
    }

    do {
        if (NULL == c) continue;
        clen = buffer_string_length(c->mem) - c->offset;
        if (0 == clen) continue;
        if (c->offset > USHRT_MAX) /*(highly unlikely)*/
            chunkqueue_compact_mem_offset(cq);

        hoff[0] = 1;                         /* number of lines */
        hoff[1] = (unsigned short)c->offset; /* base offset for all lines */
        /*hoff[2] = ...;*/                   /* offset from base for 2nd line */

        header_len = http_header_parse_hoff(c->mem->ptr + c->offset,clen,hoff);

        /* casting to (unsigned short) might truncate, and the hoff[]
         * addition might overflow, but max_request_field_size is USHRT_MAX,
         * so failure will be detected below */
        const uint32_t max_request_field_size=r->conf.max_request_field_size;
        if ((header_len ? header_len : clen) > max_request_field_size
            || hoff[0] >= sizeof(hoff)/sizeof(hoff[0])-1) {
            log_error(r->conf.errh, __FILE__, __LINE__, "%s",
                      "oversized request-header -> sending Status 431");
            r->http_status = 431; /* Request Header Fields Too Large */
            r->keep_alive = 0;
            connection_set_state(r, CON_STATE_REQUEST_END);
            return 1;
        }

        if (0 != header_len) {
            if (hoff[0] > 1) break; /* common case; request headers complete */

            if (discard_blank) { /* skip one blank line e.g. following POST */
                if (header_len == clen) continue;
                const int ch = c->mem->ptr[c->offset+header_len];
                if (ch != '\r' && ch != '\n') {
                    /* discard prior blank line if next line is not blank */
                    discard_blank = 0;
                    clen = 0;/*(for connection_read_header_more() to return c)*/
                    c = connection_discard_blank_line(cq, header_len);/*cold*/
                    continue;
                } /*(else fall through to error out in next block)*/
            }
        }

        if (((unsigned char *)c->mem->ptr)[c->offset] < 32) {
            /* expecting ASCII method beginning with alpha char
             * or HTTP/2 pseudo-header beginning with ':' */
            /*(TLS handshake begins with SYN 0x16 (decimal 22))*/
            log_error(r->conf.errh, __FILE__, __LINE__, "%s",
                      "invalid request-line -> sending Status 400");
            r->http_status = 400; /* Bad Request */
            r->keep_alive = 0;
            connection_set_state(r, CON_STATE_REQUEST_END);
            return 1;
        }
    } while ((c = connection_read_header_more(con, cq, c, clen)));

    if (keepalive_request_start) {
        if (con->bytes_read > r->bytes_read_ckpt) {
            /* update r->start_hp.tv_sec timestamp when first byte of
             * next request is received on a keep-alive connection */
            r->start_hp.tv_sec = log_epoch_secs;
            if (r->conf.high_precision_timestamps)
                log_clock_gettime_realtime(&r->start_hp);
        }
        if (pipelined_request_start && c) con->read_idle_ts = log_epoch_secs;
    }

    if (NULL == c) return 0; /* incomplete request headers */

  #ifdef __COVERITY__
    if (buffer_string_length(c->mem) < hoff[1]) {
        return 1;
    }
  #endif

    char * const hdrs = c->mem->ptr + hoff[1];

    if (con->request_count > 1) {
        /* clear buffers which may have been kept for reporting on keep-alive,
         * (e.g. mod_status) */
        request_reset_ex(r);
    }
    /* RFC7540 3.5 HTTP/2 Connection Preface
     * "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n"
     * (Connection Preface MUST be exact match)
     * If ALT-SVC used to advertise HTTP/2, then client might start
     * http connection (not TLS) sending HTTP/2 connection preface.
     * (note: intentionally checking only on initial request) */
    else if (!con->is_ssl_sock && r->conf.h2proto
             && hoff[0] == 2 && hoff[2] == 16
             && hdrs[0]=='P' && hdrs[1]=='R' && hdrs[2]=='I' && hdrs[3]==' ') {
        r->http_version = HTTP_VERSION_2;
        return 0;
    }

    r->rqst_header_len = header_len;
    if (r->conf.log_request_header)
        log_error(r->conf.errh, __FILE__, __LINE__,
          "fd: %d request-len: %d\n%.*s", con->fd,
          (int)header_len, (int)header_len, hdrs);
    http_request_headers_process(r, hdrs, hoff, con->proto_default_port);
    chunkqueue_mark_written(cq, r->rqst_header_len);
    connection_set_state(r, CON_STATE_REQUEST_END);

    if (!con->is_ssl_sock && r->conf.h2proto && 0 == r->http_status
        && h2_check_con_upgrade_h2c(r)) {
        /*(Upgrade: h2c over cleartext does not have SNI; no COMP_HTTP_HOST)*/
        r->conditional_is_valid = (1 << COMP_SERVER_SOCKET)
                                | (1 << COMP_HTTP_REMOTE_IP);
        /*connection_handle_write(r, con);*//* defer write to network */
        return 0;
    }

    return 1;
}