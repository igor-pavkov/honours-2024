static const char *set_realm(cmd_parms *cmd, void *config, const char *realm)
{
    digest_config_rec *conf = (digest_config_rec *) config;
#ifdef AP_DEBUG
    int i;

    /* check that we got random numbers */
    for (i = 0; i < SECRET_LEN; i++) {
        if (secret[i] != 0)
            break;
    }
    ap_assert(i < SECRET_LEN);
#endif

    /* The core already handles the realm, but it's just too convenient to
     * grab it ourselves too and cache some setups. However, we need to
     * let the core get at it too, which is why we decline at the end -
     * this relies on the fact that http_core is last in the list.
     */
    conf->realm = realm;

    /* we precompute the part of the nonce hash that is constant (well,
     * the host:port would be too, but that varies for .htaccess files
     * and directives outside a virtual host section)
     */
    apr_sha1_init(&conf->nonce_ctx);
    apr_sha1_update_binary(&conf->nonce_ctx, secret, SECRET_LEN);
    apr_sha1_update_binary(&conf->nonce_ctx, (const unsigned char *) realm,
                           strlen(realm));

    return DECLINE_CMD;
}