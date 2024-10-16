static void wsgi_log_script_error(request_rec *r, const char *e, const char *n)
{
    char *message = NULL;

    if (!n)
        n = r->filename;

    message = apr_psprintf(r->pool, "%s: %s", e, n);

    ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r, "%s", message);
}