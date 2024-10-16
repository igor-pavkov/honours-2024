char *am_reconstruct_url(request_rec *r)
{
    char *url;

    /* This function will construct an full url for a given path relative to
     * the root of the web site. To configure what hostname and port this
     * function will use, see the UseCanonicalName configuration directive.
     */
    url = ap_construct_url(r->pool, r->unparsed_uri, r);

    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                  "reconstruct_url: url==\"%s\", unparsed_uri==\"%s\"", url,
                  r->unparsed_uri);
    return url;
}