static const char *wsgi_set_python_eggs(cmd_parms *cmd, void *mconfig,
                                        const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);
    sconfig->python_eggs = f;

    return NULL;
}