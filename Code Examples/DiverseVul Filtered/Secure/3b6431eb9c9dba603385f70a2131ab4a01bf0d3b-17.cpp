static const char *set_uri_list(cmd_parms *cmd, void *config, const char *uri)
{
    digest_config_rec *c = (digest_config_rec *) config;
    if (c->uri_list) {
        c->uri_list[strlen(c->uri_list)-1] = '\0';
        c->uri_list = apr_pstrcat(cmd->pool, c->uri_list, " ", uri, "\"", NULL);
    }
    else {
        c->uri_list = apr_pstrcat(cmd->pool, ", domain=\"", uri, "\"", NULL);
    }
    return NULL;
}