static const char *wsgi_set_dispatch_script(cmd_parms *cmd, void *mconfig,
                                            const char *args)
{
    WSGIScriptFile *object = NULL;

    const char *option = NULL;
    const char *value = NULL;

    object = newWSGIScriptFile(cmd->pool);

    object->handler_script = ap_getword_conf(cmd->pool, &args);

    if (!object->handler_script || !*object->handler_script)
        return "Location of dispatch script not supplied.";

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI dispatch script definition.";
        }

        if (!strcmp(option, "application-group")) {
            if (!*value)
                return "Invalid name for WSGI application group.";

            object->application_group = value;
        }
        else
            return "Invalid option to WSGI dispatch script definition.";
    }

    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;
        dconfig->dispatch_script = object;
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);
        sconfig->dispatch_script = object;
    }

    wsgi_python_required = 1;

    return NULL;
}