static const char *wsgi_set_callable_object(cmd_parms *cmd, void *mconfig,
                                            const char *n)
{
    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;
        dconfig->callable_object = n;
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);
        sconfig->callable_object = n;
    }

    return NULL;
}