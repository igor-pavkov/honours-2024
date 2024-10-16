static int get_digest_rec(request_rec *r, digest_header_rec *resp)
{
    const char *auth_line;
    apr_size_t l;
    int vk = 0, vv = 0;
    char *key, *value;

    auth_line = apr_table_get(r->headers_in,
                             (PROXYREQ_PROXY == r->proxyreq)
                                 ? "Proxy-Authorization"
                                 : "Authorization");
    if (!auth_line) {
        resp->auth_hdr_sts = NO_HEADER;
        return !OK;
    }

    resp->scheme = ap_getword_white(r->pool, &auth_line);
    if (strcasecmp(resp->scheme, "Digest")) {
        resp->auth_hdr_sts = NOT_DIGEST;
        return !OK;
    }

    l = strlen(auth_line);

    key   = apr_palloc(r->pool, l+1);
    value = apr_palloc(r->pool, l+1);

    while (auth_line[0] != '\0') {

        /* find key */

        while (apr_isspace(auth_line[0])) {
            auth_line++;
        }
        vk = 0;
        while (auth_line[0] != '=' && auth_line[0] != ','
               && auth_line[0] != '\0' && !apr_isspace(auth_line[0])) {
            key[vk++] = *auth_line++;
        }
        key[vk] = '\0';
        while (apr_isspace(auth_line[0])) {
            auth_line++;
        }

        /* find value */

        vv = 0;
        if (auth_line[0] == '=') {
            auth_line++;
            while (apr_isspace(auth_line[0])) {
                auth_line++;
            }

            if (auth_line[0] == '\"') {         /* quoted string */
                auth_line++;
                while (auth_line[0] != '\"' && auth_line[0] != '\0') {
                    if (auth_line[0] == '\\' && auth_line[1] != '\0') {
                        auth_line++;            /* escaped char */
                    }
                    value[vv++] = *auth_line++;
                }
                if (auth_line[0] != '\0') {
                    auth_line++;
                }
            }
            else {                               /* token */
                while (auth_line[0] != ',' && auth_line[0] != '\0'
                       && !apr_isspace(auth_line[0])) {
                    value[vv++] = *auth_line++;
                }
            }
        }
        value[vv] = '\0';

        while (auth_line[0] != ',' && auth_line[0] != '\0') {
            auth_line++;
        }
        if (auth_line[0] != '\0') {
            auth_line++;
        }

        if (!strcasecmp(key, "username"))
            resp->username = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "realm"))
            resp->realm = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "nonce"))
            resp->nonce = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "uri"))
            resp->uri = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "response"))
            resp->digest = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "algorithm"))
            resp->algorithm = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "cnonce"))
            resp->cnonce = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "opaque"))
            resp->opaque = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "qop"))
            resp->message_qop = apr_pstrdup(r->pool, value);
        else if (!strcasecmp(key, "nc"))
            resp->nonce_count = apr_pstrdup(r->pool, value);
    }

    if (!resp->username || !resp->realm || !resp->nonce || !resp->uri
        || !resp->digest
        || (resp->message_qop && (!resp->cnonce || !resp->nonce_count))) {
        resp->auth_hdr_sts = INVALID;
        return !OK;
    }

    if (resp->opaque) {
        resp->opaque_num = (unsigned long) strtol(resp->opaque, NULL, 16);
    }

    resp->auth_hdr_sts = VALID;
    return OK;
}