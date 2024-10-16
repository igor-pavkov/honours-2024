int am_check_url(request_rec *r, const char *url)
{
    const char *i;

    for (i = url; *i; i++) {
        if (*i >= 0 && *i < ' ') {
            /* Deny all control-characters. */
            ap_log_rerror(APLOG_MARK, APLOG_ERR, HTTP_BAD_REQUEST, r,
                          "Control character detected in URL.");
            return HTTP_BAD_REQUEST;
        }
    }

    return OK;
}