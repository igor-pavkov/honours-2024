AP_DECLARE(void) ap_get_mime_headers_core(request_rec *r, apr_bucket_brigade *bb)
{
    char *last_field = NULL;
    apr_size_t last_len = 0;
    apr_size_t alloc_len = 0;
    char *field;
    char *value;
    apr_size_t len;
    int fields_read = 0;
    char *tmp_field;
    core_server_config *conf = ap_get_core_module_config(r->server->module_config);

    /*
     * Read header lines until we get the empty separator line, a read error,
     * the connection closes (EOF), reach the server limit, or we timeout.
     */
    while(1) {
        apr_status_t rv;
        int folded = 0;

        field = NULL;
        rv = ap_rgetline(&field, r->server->limit_req_fieldsize + 2,
                         &len, r, 0, bb);

        if (rv != APR_SUCCESS) {
            if (APR_STATUS_IS_TIMEUP(rv)) {
                r->status = HTTP_REQUEST_TIME_OUT;
            }
            else {
                r->status = HTTP_BAD_REQUEST;
            }

            /* ap_rgetline returns APR_ENOSPC if it fills up the buffer before
             * finding the end-of-line.  This is only going to happen if it
             * exceeds the configured limit for a field size.
             */
            if (rv == APR_ENOSPC) {
                const char *field_escaped;
                if (field) {
                    /* ensure ap_escape_html will terminate correctly */
                    field[len - 1] = '\0';
                    field_escaped = ap_escape_html(r->pool, field);
                }
                else {
                    field_escaped = field = "";
                }

                apr_table_setn(r->notes, "error-notes",
                               apr_psprintf(r->pool,
                                           "Size of a request header field "
                                           "exceeds server limit.<br />\n"
                                           "<pre>\n%.*s\n</pre>\n", 
                                           field_name_len(field_escaped),
                                           field_escaped));
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00561)
                              "Request header exceeds LimitRequestFieldSize%s"
                              "%.*s",
                              *field ? ": " : "",
                              field_name_len(field), field);
            }
            return;
        }

        if (last_field != NULL) {
            if ((len > 0) && ((*field == '\t') || *field == ' ')) {
                /* This line is a continuation of the preceding line(s),
                 * so append it to the line that we've set aside.
                 * Note: this uses a power-of-two allocator to avoid
                 * doing O(n) allocs and using O(n^2) space for
                 * continuations that span many many lines.
                 */
                apr_size_t fold_len = last_len + len + 1; /* trailing null */

                if (fold_len >= (apr_size_t)(r->server->limit_req_fieldsize)) {
                    r->status = HTTP_BAD_REQUEST;
                    /* report what we have accumulated so far before the
                     * overflow (last_field) as the field with the problem
                     */
                    apr_table_setn(r->notes, "error-notes",
                                   apr_psprintf(r->pool,
                                               "Size of a request header field "
                                               "after folding "
                                               "exceeds server limit.<br />\n"
                                               "<pre>\n%.*s\n</pre>\n", 
                                               field_name_len(last_field), 
                                               ap_escape_html(r->pool, last_field)));
                    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00562)
                                  "Request header exceeds LimitRequestFieldSize "
                                  "after folding: %.*s",
                                  field_name_len(last_field), last_field);
                    return;
                }

                if (fold_len > alloc_len) {
                    char *fold_buf;
                    alloc_len += alloc_len;
                    if (fold_len > alloc_len) {
                        alloc_len = fold_len;
                    }
                    fold_buf = (char *)apr_palloc(r->pool, alloc_len);
                    memcpy(fold_buf, last_field, last_len);
                    last_field = fold_buf;
                }
                memcpy(last_field + last_len, field, len +1); /* +1 for nul */
                last_len += len;
                folded = 1;
            }
            else /* not a continuation line */ {

                if (r->server->limit_req_fields
                    && (++fields_read > r->server->limit_req_fields)) {
                    r->status = HTTP_BAD_REQUEST;
                    apr_table_setn(r->notes, "error-notes",
                                   "The number of request header fields "
                                   "exceeds this server's limit.");
                    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00563)
                                  "Number of request headers exceeds "
                                  "LimitRequestFields");
                    return;
                }

                if (!(value = strchr(last_field, ':'))) { /* Find ':' or    */
                    r->status = HTTP_BAD_REQUEST;      /* abort bad request */
                    apr_table_setn(r->notes, "error-notes",
                                   apr_psprintf(r->pool,
                                               "Request header field is "
                                               "missing ':' separator.<br />\n"
                                               "<pre>\n%.*s</pre>\n", 
                                               (int)LOG_NAME_MAX_LEN,
                                               ap_escape_html(r->pool,
                                                              last_field)));
                    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00564)
                                  "Request header field is missing ':' "
                                  "separator: %.*s", (int)LOG_NAME_MAX_LEN,
                                  last_field);
                    return;
                }

                tmp_field = value - 1; /* last character of field-name */

                *value++ = '\0'; /* NUL-terminate at colon */

                while (*value == ' ' || *value == '\t') {
                    ++value;            /* Skip to start of value   */
                }

                /* Strip LWS after field-name: */
                while (tmp_field > last_field
                       && (*tmp_field == ' ' || *tmp_field == '\t')) {
                    *tmp_field-- = '\0';
                }

                /* Strip LWS after field-value: */
                tmp_field = last_field + last_len - 1;
                while (tmp_field > value
                       && (*tmp_field == ' ' || *tmp_field == '\t')) {
                    *tmp_field-- = '\0';
                }

                if (conf->http_conformance & AP_HTTP_CONFORMANCE_STRICT) {
                    int err = 0;

                    if (*last_field == '\0') {
                        err = HTTP_BAD_REQUEST;
                        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(02425)
                                      "Empty request header field name not allowed");
                    }
                    else if (ap_has_cntrl(last_field)) {
                        err = HTTP_BAD_REQUEST;
                        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(02426)
                                      "[HTTP strict] Request header field name contains "
                                      "control character: %.*s",
                                      (int)LOG_NAME_MAX_LEN, last_field);
                    }
                    else if (ap_has_cntrl(value)) {
                        err = HTTP_BAD_REQUEST;
                        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(02427)
                                      "Request header field '%.*s' contains "
                                      "control character", (int)LOG_NAME_MAX_LEN,
                                      last_field);
                    }
                    if (err && !(conf->http_conformance & AP_HTTP_CONFORMANCE_LOGONLY)) {
                        r->status = err;
                        return;
                    }
                }
                apr_table_addn(r->headers_in, last_field, value);

                /* reset the alloc_len so that we'll allocate a new
                 * buffer if we have to do any more folding: we can't
                 * use the previous buffer because its contents are
                 * now part of r->headers_in
                 */
                alloc_len = 0;

            } /* end if current line is not a continuation starting with tab */
        }

        /* Found a blank line, stop. */
        if (len == 0) {
            break;
        }

        /* Keep track of this line so that we can parse it on
         * the next loop iteration.  (In the folded case, last_field
         * has been updated already.)
         */
        if (!folded) {
            last_field = field;
            last_len = len;
        }
    }

    /* Combine multiple message-header fields with the same
     * field-name, following RFC 2616, 4.2.
     */
    apr_table_compress(r->headers_in, APR_OVERLAP_TABLES_MERGE);

    /* enforce LimitRequestFieldSize for merged headers */
    apr_table_do(table_do_fn_check_lengths, r, r->headers_in, NULL);
}