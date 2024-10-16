static const char *wsgi_set_socket_prefix(cmd_parms *cmd, void *mconfig,
                                         const char *arg)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    sconfig->socket_prefix = ap_server_root_relative(cmd->pool, arg);

    if (!sconfig->socket_prefix) {
        return apr_pstrcat(cmd->pool, "Invalid WSGISocketPrefix '",
                           arg, "'.", NULL);
    }

    return NULL;
}