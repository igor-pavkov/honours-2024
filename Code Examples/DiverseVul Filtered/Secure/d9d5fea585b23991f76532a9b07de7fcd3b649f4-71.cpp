static const char *wsgi_set_enable_sendfile(cmd_parms *cmd, void *mconfig,
                                            const char *f)
{
    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        if (strcasecmp(f, "Off") == 0)
            dconfig->enable_sendfile = 0;
        else if (strcasecmp(f, "On") == 0)
            dconfig->enable_sendfile = 1;
        else
            return "WSGIEnableSendfile must be one of: Off | On";
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        if (strcasecmp(f, "Off") == 0)
            sconfig->enable_sendfile = 0;
        else if (strcasecmp(f, "On") == 0)
            sconfig->enable_sendfile = 1;
        else
            return "WSGIEnableSendfile must be one of: Off | On";
    }

    return NULL;
}