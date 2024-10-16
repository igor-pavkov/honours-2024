apr_status_t h2_request_rcreate(h2_request **preq, apr_pool_t *pool, 
                                request_rec *r)
{
    h2_request *req;
    const char *scheme, *authority, *path;
    h1_ctx x;
    
    *preq = NULL;
    scheme = apr_pstrdup(pool, r->parsed_uri.scheme? r->parsed_uri.scheme
              : ap_http_scheme(r));
    authority = apr_pstrdup(pool, r->hostname);
    path = apr_uri_unparse(pool, &r->parsed_uri, APR_URI_UNP_OMITSITEPART);
    
    if (!r->method || !scheme || !r->hostname || !path) {
        return APR_EINVAL;
    }

    if (!ap_strchr_c(authority, ':') && r->server && r->server->port) {
        apr_port_t defport = apr_uri_port_of_scheme(scheme);
        if (defport != r->server->port) {
            /* port info missing and port is not default for scheme: append */
            authority = apr_psprintf(pool, "%s:%d", authority,
                                     (int)r->server->port);
        }
    }
    
    req = apr_pcalloc(pool, sizeof(*req));
    req->method    = apr_pstrdup(pool, r->method);
    req->scheme    = scheme;
    req->authority = authority;
    req->path      = path;
    req->headers   = apr_table_make(pool, 10);
    if (r->server) {
        req->serialize = h2_config_rgeti(r, H2_CONF_SER_HEADERS);
    }

    x.pool = pool;
    x.headers = req->headers;
    x.status = APR_SUCCESS;
    apr_table_do(set_h1_header, &x, r->headers_in, NULL);
    
    *preq = req;
    return x.status;
}