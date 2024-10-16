static int table_do_fn_check_lengths(void *r_, const char *key,
                                     const char *value)
{
    request_rec *r = r_;
    if (value == NULL || r->server->limit_req_fieldsize >= strlen(value) )
        return 1;

    r->status = HTTP_BAD_REQUEST;
    apr_table_setn(r->notes, "error-notes",
                   apr_pstrcat(r->pool, "Size of a request header field "
                               "after merging exceeds server limit.<br />"
                               "\n<pre>\n",
                               ap_escape_html(r->pool, key),
                               "</pre>\n", NULL));
    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00560) "Request header "
                  "exceeds LimitRequestFieldSize after merging: %s", key);
    return 0;
}