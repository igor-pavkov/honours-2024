static const char *wsgi_set_restrict_process(cmd_parms *cmd, void *mconfig,
                                             const char *args)
{
    apr_table_t *index = apr_table_make(cmd->pool, 5);

    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        dconfig->restrict_process = index;
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        sconfig->restrict_process = index;
    }

    while (*args) {
        const char *option;

        option = ap_getword_conf(cmd->pool, &args);

        if (!strcmp(option, "%{GLOBAL}"))
            option = "";

        apr_table_setn(index, option, option);
    }

    return NULL;
}