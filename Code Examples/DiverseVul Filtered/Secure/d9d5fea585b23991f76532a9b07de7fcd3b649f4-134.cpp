static const char *wsgi_set_restrict_stdout(cmd_parms *cmd, void *mconfig,
                                            const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    if (strcasecmp(f, "Off") == 0)
        sconfig->restrict_stdout = 0;
    else if (strcasecmp(f, "On") == 0)
        sconfig->restrict_stdout = 1;
    else
        return "WSGIRestrictStdout must be one of: Off | On";

    return NULL;
}