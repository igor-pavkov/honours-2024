static const char *wsgi_set_error_override(cmd_parms *cmd, void *mconfig,
                                           const char *f)
{
    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        if (strcasecmp(f, "Off") == 0)
            dconfig->error_override = 0;
        else if (strcasecmp(f, "On") == 0)
            dconfig->error_override = 1;
        else
            return "WSGIErrorOverride must be one of: Off | On";
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        if (strcasecmp(f, "Off") == 0)
            sconfig->error_override = 0;
        else if (strcasecmp(f, "On") == 0)
            sconfig->error_override = 1;
        else
            return "WSGIErrorOverride must be one of: Off | On";
    }

    return NULL;
}