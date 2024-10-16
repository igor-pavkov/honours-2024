static void copy_uri_components(apr_uri_t *dst,
                                apr_uri_t *src, request_rec *r) {
    if (src->scheme && src->scheme[0] != '\0') {
        dst->scheme = src->scheme;
    }
    else {
        dst->scheme = (char *) "http";
    }

    if (src->hostname && src->hostname[0] != '\0') {
        dst->hostname = apr_pstrdup(r->pool, src->hostname);
        ap_unescape_url(dst->hostname);
    }
    else {
        dst->hostname = (char *) ap_get_server_name(r);
    }

    if (src->port_str && src->port_str[0] != '\0') {
        dst->port = src->port;
    }
    else {
        dst->port = ap_get_server_port(r);
    }

    if (src->path && src->path[0] != '\0') {
        dst->path = apr_pstrdup(r->pool, src->path);
        ap_unescape_url(dst->path);
    }
    else {
        dst->path = src->path;
    }

    if (src->query && src->query[0] != '\0') {
        dst->query = apr_pstrdup(r->pool, src->query);
        ap_unescape_url(dst->query);
    }
    else {
        dst->query = src->query;
    }

    dst->hostinfo = src->hostinfo;
}