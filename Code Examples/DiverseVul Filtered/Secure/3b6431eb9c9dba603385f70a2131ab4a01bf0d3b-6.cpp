static void note_digest_auth_failure(request_rec *r,
                                     const digest_config_rec *conf,
                                     digest_header_rec *resp, int stale)
{
    const char   *qop, *opaque, *opaque_param, *domain, *nonce;

    /* Setup qop */
    if (apr_is_empty_array(conf->qop_list)) {
        qop = ", qop=\"auth\"";
    }
    else if (!strcasecmp(*(const char **)(conf->qop_list->elts), "none")) {
        qop = "";
    }
    else {
        qop = apr_pstrcat(r->pool, ", qop=\"",
                                   apr_array_pstrcat(r->pool, conf->qop_list, ','),
                                   "\"",
                                   NULL);
    }

    /* Setup opaque */

    if (resp->opaque == NULL) {
        /* new client */
        if ((conf->check_nc || conf->nonce_lifetime == 0)
            && (resp->client = gen_client(r)) != NULL) {
            opaque = ltox(r->pool, resp->client->key);
        }
        else {
            opaque = "";                /* opaque not needed */
        }
    }
    else if (resp->client == NULL) {
        /* client info was gc'd */
        resp->client = gen_client(r);
        if (resp->client != NULL) {
            opaque = ltox(r->pool, resp->client->key);
            stale = 1;
            client_list->num_renewed++;
        }
        else {
            opaque = "";                /* ??? */
        }
    }
    else {
        opaque = resp->opaque;
        /* we're generating a new nonce, so reset the nonce-count */
        resp->client->nonce_count = 0;
    }

    if (opaque[0]) {
        opaque_param = apr_pstrcat(r->pool, ", opaque=\"", opaque, "\"", NULL);
    }
    else {
        opaque_param = NULL;
    }

    /* Setup nonce */

    nonce = gen_nonce(r->pool, r->request_time, opaque, r->server, conf);
    if (resp->client && conf->nonce_lifetime == 0) {
        memcpy(resp->client->last_nonce, nonce, NONCE_LEN+1);
    }

    /* setup domain attribute. We want to send this attribute wherever
     * possible so that the client won't send the Authorization header
     * unnecessarily (it's usually > 200 bytes!).
     */


    /* don't send domain
     * - for proxy requests
     * - if it's not specified
     */
    if (r->proxyreq || !conf->uri_list) {
        domain = NULL;
    }
    else {
        domain = conf->uri_list;
    }

    apr_table_mergen(r->err_headers_out,
                     (PROXYREQ_PROXY == r->proxyreq)
                         ? "Proxy-Authenticate" : "WWW-Authenticate",
                     apr_psprintf(r->pool, "Digest realm=\"%s\", "
                                  "nonce=\"%s\", algorithm=%s%s%s%s%s",
                                  ap_auth_name(r), nonce, conf->algorithm,
                                  opaque_param ? opaque_param : "",
                                  domain ? domain : "",
                                  stale ? ", stale=true" : "", qop));

}