static const char *wsgi_set_restrict_embedded(cmd_parms *cmd, void *mconfig,
                                              const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    if (strcasecmp(f, "Off") == 0)
        sconfig->restrict_embedded = 0;
    else if (strcasecmp(f, "On") == 0)
        sconfig->restrict_embedded = 1;
    else
        return "WSGIRestrictEmbedded must be one of: Off | On";

    if (sconfig->restrict_embedded) {
        if (wsgi_python_required == -1)
            wsgi_python_required = 0;
    }

    return NULL;
}