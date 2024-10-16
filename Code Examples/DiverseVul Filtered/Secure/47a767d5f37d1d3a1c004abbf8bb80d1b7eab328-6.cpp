const char *am_get_mime_body(request_rec *r, const char *mime) 
{
    const char lflf[] = "\n\n";
    const char *body;
    apr_size_t body_len;

    if ((body = strstr(mime, lflf)) == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "No MIME body");
        return NULL;
    }

    body += strlen(lflf);

    /* Strip tralling \n */
    if ((body_len = strlen(body)) >= 1) {
        if (body[body_len - 1] == '\n') 
            body = apr_pstrmemdup(r->pool, body, body_len - 1);
    }

    /* Turn back LF into CRLF */
    return am_add_cr(r, body);
}