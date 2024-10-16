static const char *wsgi_set_case_sensitivity(cmd_parms *cmd, void *mconfig,
                                           const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    if (strcasecmp(f, "Off") == 0)
        sconfig->case_sensitivity = 0;
    else if (strcasecmp(f, "On") == 0)
        sconfig->case_sensitivity = 1;
    else
        return "WSGICaseSensitivity must be one of: Off | On";

    return NULL;
}