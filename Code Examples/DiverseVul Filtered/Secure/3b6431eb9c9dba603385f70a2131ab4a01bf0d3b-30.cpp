static const char *set_nonce_lifetime(cmd_parms *cmd, void *config,
                                      const char *t)
{
    char *endptr;
    long  lifetime;

    lifetime = strtol(t, &endptr, 10);
    if (endptr < (t+strlen(t)) && !apr_isspace(*endptr)) {
        return apr_pstrcat(cmd->pool,
                           "Invalid time in AuthDigestNonceLifetime: ",
                           t, NULL);
    }

    ((digest_config_rec *) config)->nonce_lifetime = apr_time_from_sec(lifetime);
    return NULL;
}