static int authenticate_digest_user(request_rec *r)
{
    digest_config_rec *conf;
    digest_header_rec *resp;
    request_rec       *mainreq;
    const char        *t;
    int                res;
    authn_status       return_code;

    /* do we require Digest auth for this URI? */

    if (!(t = ap_auth_type(r)) || strcasecmp(t, "Digest")) {
        return DECLINED;
    }

    if (!ap_auth_name(r)) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01780)
                      "need AuthName: %s", r->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }


    /* get the client response and mark */

    mainreq = r;
    while (mainreq->main != NULL) {
        mainreq = mainreq->main;
    }
    while (mainreq->prev != NULL) {
        mainreq = mainreq->prev;
    }
    resp = (digest_header_rec *) ap_get_module_config(mainreq->request_config,
                                                      &auth_digest_module);
    resp->needed_auth = 1;


    /* get our conf */

    conf = (digest_config_rec *) ap_get_module_config(r->per_dir_config,
                                                      &auth_digest_module);


    /* check for existence and syntax of Auth header */

    if (resp->auth_hdr_sts != VALID) {
        if (resp->auth_hdr_sts == NOT_DIGEST) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01781)
                          "client used wrong authentication scheme `%s': %s",
                          resp->scheme, r->uri);
        }
        else if (resp->auth_hdr_sts == INVALID) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01782)
                          "missing user, realm, nonce, uri, digest, "
                          "cnonce, or nonce_count in authorization header: %s",
                          r->uri);
        }
        /* else (resp->auth_hdr_sts == NO_HEADER) */
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    r->user         = (char *) resp->username;
    r->ap_auth_type = (char *) "Digest";

    /* check the auth attributes */

    if (strcmp(resp->uri, resp->raw_request_uri)) {
        /* Hmm, the simple match didn't work (probably a proxy modified the
         * request-uri), so lets do a more sophisticated match
         */
        apr_uri_t r_uri, d_uri;

        copy_uri_components(&r_uri, resp->psd_request_uri, r);
        if (apr_uri_parse(r->pool, resp->uri, &d_uri) != APR_SUCCESS) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01783)
                          "invalid uri <%s> in Authorization header",
                          resp->uri);
            return HTTP_BAD_REQUEST;
        }

        if (d_uri.hostname) {
            ap_unescape_url(d_uri.hostname);
        }
        if (d_uri.path) {
            ap_unescape_url(d_uri.path);
        }

        if (d_uri.query) {
            ap_unescape_url(d_uri.query);
        }
        else if (r_uri.query) {
            /* MSIE compatibility hack.  MSIE has some RFC issues - doesn't
             * include the query string in the uri Authorization component
             * or when computing the response component.  the second part
             * works out ok, since we can hash the header and get the same
             * result.  however, the uri from the request line won't match
             * the uri Authorization component since the header lacks the
             * query string, leaving us incompatible with a (broken) MSIE.
             *
             * the workaround is to fake a query string match if in the proper
             * environment - BrowserMatch MSIE, for example.  the cool thing
             * is that if MSIE ever fixes itself the simple match ought to
             * work and this code won't be reached anyway, even if the
             * environment is set.
             */

            if (apr_table_get(r->subprocess_env,
                              "AuthDigestEnableQueryStringHack")) {

                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(01784)
                              "applying AuthDigestEnableQueryStringHack "
                              "to uri <%s>", resp->raw_request_uri);

               d_uri.query = r_uri.query;
            }
        }

        if (r->method_number == M_CONNECT) {
            if (!r_uri.hostinfo || strcmp(resp->uri, r_uri.hostinfo)) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01785)
                              "uri mismatch - <%s> does not match "
                              "request-uri <%s>", resp->uri, r_uri.hostinfo);
                return HTTP_BAD_REQUEST;
            }
        }
        else if (
            /* check hostname matches, if present */
            (d_uri.hostname && d_uri.hostname[0] != '\0'
              && strcasecmp(d_uri.hostname, r_uri.hostname))
            /* check port matches, if present */
            || (d_uri.port_str && d_uri.port != r_uri.port)
            /* check that server-port is default port if no port present */
            || (d_uri.hostname && d_uri.hostname[0] != '\0'
                && !d_uri.port_str && r_uri.port != ap_default_port(r))
            /* check that path matches */
            || (d_uri.path != r_uri.path
                /* either exact match */
                && (!d_uri.path || !r_uri.path
                    || strcmp(d_uri.path, r_uri.path))
                /* or '*' matches empty path in scheme://host */
                && !(d_uri.path && !r_uri.path && resp->psd_request_uri->hostname
                    && d_uri.path[0] == '*' && d_uri.path[1] == '\0'))
            /* check that query matches */
            || (d_uri.query != r_uri.query
                && (!d_uri.query || !r_uri.query
                    || strcmp(d_uri.query, r_uri.query)))
            ) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01786)
                          "uri mismatch - <%s> does not match "
                          "request-uri <%s>", resp->uri, resp->raw_request_uri);
            return HTTP_BAD_REQUEST;
        }
    }

    if (resp->opaque && resp->opaque_num == 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01787)
                      "received invalid opaque - got `%s'",
                      resp->opaque);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    if (!conf->realm) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02533)
                      "realm mismatch - got `%s' but no realm specified",
                      resp->realm);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    if (!resp->realm || strcmp(resp->realm, conf->realm)) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01788)
                      "realm mismatch - got `%s' but expected `%s'",
                      resp->realm, conf->realm);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    if (resp->algorithm != NULL
        && strcasecmp(resp->algorithm, "MD5")) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01789)
                      "unknown algorithm `%s' received: %s",
                      resp->algorithm, r->uri);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    return_code = get_hash(r, r->user, conf, &resp->ha1);

    if (return_code == AUTH_USER_NOT_FOUND) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01790)
                      "user `%s' in realm `%s' not found: %s",
                      r->user, conf->realm, r->uri);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }
    else if (return_code == AUTH_USER_FOUND) {
        /* we have a password, so continue */
    }
    else if (return_code == AUTH_DENIED) {
        /* authentication denied in the provider before attempting a match */
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01791)
                      "user `%s' in realm `%s' denied by provider: %s",
                      r->user, conf->realm, r->uri);
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }
    else {
        /* AUTH_GENERAL_ERROR (or worse)
         * We'll assume that the module has already said what its error
         * was in the logs.
         */
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (resp->message_qop == NULL) {
        /* old (rfc-2069) style digest */
        if (strcmp(resp->digest, old_digest(r, resp))) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01792)
                          "user %s: password mismatch: %s", r->user,
                          r->uri);
            note_digest_auth_failure(r, conf, resp, 0);
            return HTTP_UNAUTHORIZED;
        }
    }
    else {
        const char *exp_digest;
        int match = 0, idx;
        const char **tmp = (const char **)(conf->qop_list->elts);
        for (idx = 0; idx < conf->qop_list->nelts; idx++) {
            if (!strcasecmp(*tmp, resp->message_qop)) {
                match = 1;
                break;
            }
            ++tmp;
        }

        if (!match
            && !(apr_is_empty_array(conf->qop_list)
                 && !strcasecmp(resp->message_qop, "auth"))) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01793)
                          "invalid qop `%s' received: %s",
                          resp->message_qop, r->uri);
            note_digest_auth_failure(r, conf, resp, 0);
            return HTTP_UNAUTHORIZED;
        }

        exp_digest = new_digest(r, resp);
        if (!exp_digest) {
            /* we failed to allocate a client struct */
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if (strcmp(resp->digest, exp_digest)) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01794)
                          "user %s: password mismatch: %s", r->user,
                          r->uri);
            note_digest_auth_failure(r, conf, resp, 0);
            return HTTP_UNAUTHORIZED;
        }
    }

    if (check_nc(r, resp, conf) != OK) {
        note_digest_auth_failure(r, conf, resp, 0);
        return HTTP_UNAUTHORIZED;
    }

    /* Note: this check is done last so that a "stale=true" can be
       generated if the nonce is old */
    if ((res = check_nonce(r, resp, conf))) {
        return res;
    }

    return OK;
}