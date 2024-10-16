static const char *set_qop(cmd_parms *cmd, void *config, const char *op)
{
    digest_config_rec *conf = (digest_config_rec *) config;

    if (!strcasecmp(op, "none")) {
        apr_array_clear(conf->qop_list);
        *(const char **)apr_array_push(conf->qop_list) = "none";
        return NULL;
    }

    if (!strcasecmp(op, "auth-int")) {
        return "AuthDigestQop auth-int is not implemented";
    }
    else if (strcasecmp(op, "auth")) {
        return apr_pstrcat(cmd->pool, "Unrecognized qop: ", op, NULL);
    }

    *(const char **)apr_array_push(conf->qop_list) = op;

    return NULL;
}