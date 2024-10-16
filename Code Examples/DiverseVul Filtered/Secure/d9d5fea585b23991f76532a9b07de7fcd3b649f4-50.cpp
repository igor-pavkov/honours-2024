static const char *wsgi_set_auth_group_script(cmd_parms *cmd, void *mconfig,
                                               const char *args)
{
    WSGIDirectoryConfig *dconfig = NULL;
    WSGIScriptFile *object = NULL;

    const char *option = NULL;
    const char *value = NULL;

    object = newWSGIScriptFile(cmd->pool);

    object->handler_script = ap_getword_conf(cmd->pool, &args);

    if (!object->handler_script || !*object->handler_script)
        return "Location of auth group script not supplied.";

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI auth group script definition.";
        }

        if (!strcmp(option, "application-group")) {
            if (!*value)
                return "Invalid name for WSGI application group.";

            object->application_group = value;
        }
        else
            return "Invalid option to WSGI auth group script definition.";
    }

    dconfig = (WSGIDirectoryConfig *)mconfig;
    dconfig->auth_group_script = object;

    wsgi_python_required = 1;

    return NULL;
}