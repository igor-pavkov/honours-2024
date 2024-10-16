static int add_auth_info(request_rec *r)
{
    const digest_config_rec *conf =
                (digest_config_rec *) ap_get_module_config(r->per_dir_config,
                                                           &auth_digest_module);
    digest_header_rec *resp =
                (digest_header_rec *) ap_get_module_config(r->request_config,
                                                           &auth_digest_module);
    const char *ai = NULL, *nextnonce = "";

    if (resp == NULL || !resp->needed_auth || conf == NULL) {
        return OK;
    }

    /* 2069-style entity-digest is not supported (it's too hard, and
     * there are no clients which support 2069 but not 2617). */

    /* setup nextnonce
     */
    if (conf->nonce_lifetime > 0) {
        /* send nextnonce if current nonce will expire in less than 30 secs */
        if ((r->request_time - resp->nonce_time) > (conf->nonce_lifetime-NEXTNONCE_DELTA)) {
            nextnonce = apr_pstrcat(r->pool, ", nextnonce=\"",
                                   gen_nonce(r->pool, r->request_time,
                                             resp->opaque, r->server, conf),
                                   "\"", NULL);
            if (resp->client)
                resp->client->nonce_count = 0;
        }
    }
    else if (conf->nonce_lifetime == 0 && resp->client) {
        const char *nonce = gen_nonce(r->pool, 0, resp->opaque, r->server,
                                      conf);
        nextnonce = apr_pstrcat(r->pool, ", nextnonce=\"", nonce, "\"", NULL);
        memcpy(resp->client->last_nonce, nonce, NONCE_LEN+1);
    }
    /* else nonce never expires, hence no nextnonce */


    /* do rfc-2069 digest
     */
    if (!apr_is_empty_array(conf->qop_list) &&
        !strcasecmp(*(const char **)(conf->qop_list->elts), "none")
        && resp->message_qop == NULL) {
        /* use only RFC-2069 format */
        ai = nextnonce;
    }
    else {
        const char *resp_dig, *ha1, *a2, *ha2;

        /* calculate rspauth attribute
         */
        ha1 = resp->ha1;

        a2 = apr_pstrcat(r->pool, ":", resp->uri, NULL);
        ha2 = ap_md5(r->pool, (const unsigned char *)a2);

        resp_dig = ap_md5(r->pool,
                          (unsigned char *)apr_pstrcat(r->pool, ha1, ":",
                                                       resp->nonce, ":",
                                                       resp->nonce_count, ":",
                                                       resp->cnonce, ":",
                                                       resp->message_qop ?
                                                         resp->message_qop : "",
                                                       ":", ha2, NULL));

        /* assemble Authentication-Info header
         */
        ai = apr_pstrcat(r->pool,
                         "rspauth=\"", resp_dig, "\"",
                         nextnonce,
                         resp->cnonce ? ", cnonce=\"" : "",
                         resp->cnonce
                           ? ap_escape_quotes(r->pool, resp->cnonce)
                           : "",
                         resp->cnonce ? "\"" : "",
                         resp->nonce_count ? ", nc=" : "",
                         resp->nonce_count ? resp->nonce_count : "",
                         resp->message_qop ? ", qop=" : "",
                         resp->message_qop ? resp->message_qop : "",
                         NULL);
    }

    if (ai && ai[0]) {
        apr_table_mergen(r->headers_out,
                         (PROXYREQ_PROXY == r->proxyreq)
                             ? "Proxy-Authentication-Info"
                             : "Authentication-Info",
                         ai);
    }

    return OK;
}