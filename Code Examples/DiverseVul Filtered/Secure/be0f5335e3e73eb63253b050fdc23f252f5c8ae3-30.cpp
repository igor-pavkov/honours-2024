AP_DECLARE(void) ap_send_interim_response(request_rec *r, int send_headers)
{
    hdr_ptr x;
    char *status_line = NULL;
    request_rec *rr;

    if (r->proto_num < HTTP_VERSION(1,1)) {
        /* don't send interim response to HTTP/1.0 Client */
        return;
    }
    if (!ap_is_HTTP_INFO(r->status)) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(00575)
                      "Status is %d - not sending interim response", r->status);
        return;
    }
    if ((r->status == HTTP_CONTINUE) && !r->expecting_100) {
        /*
         * Don't send 100-Continue when there was no Expect: 100-continue
         * in the request headers. For origin servers this is a SHOULD NOT
         * for proxies it is a MUST NOT according to RFC 2616 8.2.3
         */
        return;
    }

    /* if we send an interim response, we're no longer in a state of
     * expecting one.  Also, this could feasibly be in a subrequest,
     * so we need to propagate the fact that we responded.
     */
    for (rr = r; rr != NULL; rr = rr->main) {
        rr->expecting_100 = 0;
    }

    status_line = apr_pstrcat(r->pool, AP_SERVER_PROTOCOL, " ", r->status_line, CRLF, NULL);
    ap_xlate_proto_to_ascii(status_line, strlen(status_line));

    x.f = r->connection->output_filters;
    x.bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);

    ap_fputs(x.f, x.bb, status_line);
    if (send_headers) {
        apr_table_do(send_header, &x, r->headers_out, NULL);
        apr_table_clear(r->headers_out);
    }
    ap_fputs(x.f, x.bb, CRLF_ASCII);
    ap_fflush(x.f, x.bb);
    apr_brigade_destroy(x.bb);
}