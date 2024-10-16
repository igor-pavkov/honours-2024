static const char *wsgi_set_script_reloading(cmd_parms *cmd, void *mconfig,
                                             const char *f)
{
    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        if (strcasecmp(f, "Off") == 0)
            dconfig->script_reloading = 0;
        else if (strcasecmp(f, "On") == 0)
            dconfig->script_reloading = 1;
        else
            return "WSGIScriptReloading must be one of: Off | On";
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        if (strcasecmp(f, "Off") == 0)
            sconfig->script_reloading = 0;
        else if (strcasecmp(f, "On") == 0)
            sconfig->script_reloading = 1;
        else
            return "WSGIScriptReloading must be one of: Off | On";
    }

    return NULL;
}