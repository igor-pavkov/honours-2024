static const char *wsgi_set_chunked_request(cmd_parms *cmd, void *mconfig,
                                            const char *f)
{
    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        if (strcasecmp(f, "Off") == 0)
            dconfig->chunked_request = 0;
        else if (strcasecmp(f, "On") == 0)
            dconfig->chunked_request = 1;
        else
            return "WSGIChunkedRequest must be one of: Off | On";
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        if (strcasecmp(f, "Off") == 0)
            sconfig->chunked_request = 0;
        else if (strcasecmp(f, "On") == 0)
            sconfig->chunked_request = 1;
        else
            return "WSGIChunkedRequest must be one of: Off | On";
    }

    return NULL;
}