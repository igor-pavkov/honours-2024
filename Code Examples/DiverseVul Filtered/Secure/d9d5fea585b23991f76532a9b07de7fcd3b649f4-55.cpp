static const char *wsgi_add_python_warnings(cmd_parms *cmd, void *mconfig,
                                            const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    char **entry = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    if (!sconfig->python_warnings) {
        sconfig->python_warnings = apr_array_make(sconfig->pool, 5,
                                                  sizeof(char*));
    }

    entry = (char **)apr_array_push(sconfig->python_warnings);
    *entry = apr_pstrdup(sconfig->pool, f);

    return NULL;
}