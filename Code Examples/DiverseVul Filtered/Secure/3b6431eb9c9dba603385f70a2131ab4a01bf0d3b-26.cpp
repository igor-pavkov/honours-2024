static authn_status get_hash(request_rec *r, const char *user,
                             digest_config_rec *conf, const char **rethash)
{
    authn_status auth_result;
    char *password;
    authn_provider_list *current_provider;

    current_provider = conf->providers;
    do {
        const authn_provider *provider;

        /* For now, if a provider isn't set, we'll be nice and use the file
         * provider.
         */
        if (!current_provider) {
            provider = ap_lookup_provider(AUTHN_PROVIDER_GROUP,
                                          AUTHN_DEFAULT_PROVIDER,
                                          AUTHN_PROVIDER_VERSION);

            if (!provider || !provider->get_realm_hash) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01770)
                              "No Authn provider configured");
                auth_result = AUTH_GENERAL_ERROR;
                break;
            }
            apr_table_setn(r->notes, AUTHN_PROVIDER_NAME_NOTE, AUTHN_DEFAULT_PROVIDER);
        }
        else {
            provider = current_provider->provider;
            apr_table_setn(r->notes, AUTHN_PROVIDER_NAME_NOTE, current_provider->provider_name);
        }


        /* We expect the password to be md5 hash of user:realm:password */
        auth_result = provider->get_realm_hash(r, user, conf->realm,
                                               &password);

        apr_table_unset(r->notes, AUTHN_PROVIDER_NAME_NOTE);

        /* Something occurred.  Stop checking. */
        if (auth_result != AUTH_USER_NOT_FOUND) {
            break;
        }

        /* If we're not really configured for providers, stop now. */
        if (!conf->providers) {
           break;
        }

        current_provider = current_provider->next;
    } while (current_provider);

    if (auth_result == AUTH_USER_FOUND) {
        *rethash = password;
    }

    return auth_result;
}