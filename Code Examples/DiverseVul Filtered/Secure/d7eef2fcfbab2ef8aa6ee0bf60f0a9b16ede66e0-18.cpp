sync_cookie_parse(char *cookie, PRBool *cookie_refresh, PRBool *allow_openldap_compat)
{
    char *p = NULL;
    char *q = NULL;
    Sync_Cookie *sc = NULL;

    /* This is an rfc compliant initial refresh request */
    if (cookie == NULL || *cookie == '\0') {
        *cookie_refresh = PR_TRUE;
        return NULL;
    }

    /* get ready to parse. */
    p = q = cookie;

    sc = (Sync_Cookie *)slapi_ch_calloc(1, sizeof(Sync_Cookie));
    if (strncmp(cookie, "rid=", 4) == 0) {
        if (*allow_openldap_compat != PR_TRUE) {
            slapi_log_err(SLAPI_LOG_ERR, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_parse - An openldap sync request was made, but " SYNC_ALLOW_OPENLDAP_COMPAT " is false\n");
            slapi_log_err(SLAPI_LOG_ERR, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_parse - To enable this run 'dsconf <instance> plugin contentsync set --allow-openldap on'\n");
            goto error_return;
        }
        /*
         * We are in openldap mode.
         * The cookies are:
         * rid=123,csn=20200525051329.534174Z#000000#000#000000
         */
        sc->openldap_compat = PR_TRUE;
        p = strchr(q, ',');
        if (p == NULL) {
            /* No CSN following the rid, must be an init request. */
            *cookie_refresh = PR_TRUE;
            /* We need to keep the client rid though */
            sc->cookie_client_signature = slapi_ch_strdup(q);
            /* server sig and change info do not need to be set. */
            sc->cookie_server_signature = NULL;
            sc->cookie_change_info = 0;
        } else {
            /* Ensure that this really is a csn= */
            if (strncmp(p, ",csn=", 5) != 0) {
                /* Yeah nahhhhhhh */
                goto error_return;
            }
            /* We dont care about the remainder after the . */
            if (strlen(p) < 20) {
                /* Probably a corrupt CSN. We need at least 20 chars. */
                goto error_return;
            }
            /*
             * Replace the , with a '\0' This makes q -> p a str of the rid.
             * rid=123,csn=19700101001640.000000Z#000000#000#000000
             * ^      ^
             * q      p
             * rid=123\0csn=19700101001640.000000Z#000000#000#000000
             */
            PR_ASSERT(p[0] == ',');
            p[0] = '\0';
            /*
             * Now terminate the ulong which is our change num so we can parse it.
             * rid=123\0csn=19700101001640.000000Z#000000#000#000000
             * ^       ^                  ^
             * q       p[0]               p[19]
             * rid=123\0csn=19700101001640\0...
             */
            PR_ASSERT(p[19] == '.');
            p[19] = '\0';
            /*
             * And move the pointer up to the start of the int we need to parse.
             * rid=123\0csn=19700101001640\0...
             * ^       ^
             * q       p +5 -->
             * rid=123\0csn=19700101001640\0...
             * ^            ^
             * q            p
             */
            p = p + 5;
            PR_ASSERT(strlen(p) == 14);
            /* We are now ready to parse the csn and create a cookie! */
            sc->cookie_client_signature = slapi_ch_strdup(q);
            sc->cookie_server_signature = NULL;
            /* Get the change number from the string */
            sc->cookie_change_info = sync_olcsn2ulong(p);
            if (SYNC_INVALID_CHANGENUM == sc->cookie_change_info) {
                /* Sad trombone */
                goto error_return;
            }
            /* Done! 🎉 */
        }
    } else {
        /*
         * Format of the 389 cookie: server_signature#client_signature#change_info_number
         * If the cookie is malformed, NULL is returned.
         */
        p = strchr(q, '#');
        if (p) {
            *p = '\0';
            sc->cookie_server_signature = slapi_ch_strdup(q);
            q = p + 1;
            p = strchr(q, '#');
            if (p) {
                *p = '\0';
                sc->cookie_client_signature = slapi_ch_strdup(q);
                sc->cookie_change_info = sync_number2ulong(p + 1);
                if (SYNC_INVALID_CHANGENUM == sc->cookie_change_info) {
                    goto error_return;
                }
            } else {
                goto error_return;
            }
        }
    }
    return (sc);
error_return:
    slapi_ch_free_string(&(sc->cookie_client_signature));
    slapi_ch_free_string(&(sc->cookie_server_signature));
    slapi_ch_free((void **)&sc);
    return NULL;
}