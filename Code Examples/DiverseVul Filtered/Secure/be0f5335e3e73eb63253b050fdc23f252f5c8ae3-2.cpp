AP_DECLARE(int) ap_get_basic_auth_pw(request_rec *r, const char **pw)
{
    const char *auth_line = apr_table_get(r->headers_in,
                                          (PROXYREQ_PROXY == r->proxyreq)
                                              ? "Proxy-Authorization"
                                              : "Authorization");
    const char *t;

    if (!(t = ap_auth_type(r)) || strcasecmp(t, "Basic"))
        return DECLINED;

    if (!ap_auth_name(r)) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00572) 
                      "need AuthName: %s", r->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!auth_line) {
        ap_note_auth_failure(r);
        return HTTP_UNAUTHORIZED;
    }

    if (strcasecmp(ap_getword(r->pool, &auth_line, ' '), "Basic")) {
        /* Client tried to authenticate using wrong auth scheme */
        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00573)
                      "client used wrong authentication scheme: %s", r->uri);
        ap_note_auth_failure(r);
        return HTTP_UNAUTHORIZED;
    }

    while (*auth_line == ' ' || *auth_line == '\t') {
        auth_line++;
    }

    t = ap_pbase64decode(r->pool, auth_line);
    r->user = ap_getword_nulls (r->pool, &t, ':');
    r->ap_auth_type = "Basic";

    *pw = t;

    return OK;
}