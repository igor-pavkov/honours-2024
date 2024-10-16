static const char *wsgi_add_handler_script(cmd_parms *cmd, void *mconfig,
                                           const char *args)
{
    WSGIServerConfig *sconfig = NULL;
    WSGIDirectoryConfig *dconfig = NULL;
    WSGIScriptFile *object = NULL;

    const char *name = NULL;
    const char *option = NULL;
    const char *value = NULL;

    name = ap_getword_conf(cmd->pool, &args);

    if (!name || !*name)
        return "Name for handler script not supplied.";

    object = newWSGIScriptFile(cmd->pool);

    object->handler_script = ap_getword_conf(cmd->pool, &args);

    if (!object->handler_script || !*object->handler_script)
        return "Location of handler script not supplied.";

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI handler script definition.";
        }

        if (!strcmp(option, "process-group")) {
            if (!*value)
                return "Invalid name for WSGI process group.";

            object->process_group = value;
        }
        else if (!strcmp(option, "application-group")) {
            if (!*value)
                return "Invalid name for WSGI application group.";

            object->application_group = value;
        }
        else if (!strcmp(option, "pass-authorization")) {
            if (!*value)
                return "Invalid value for authorization flag.";

            if (strcasecmp(value, "Off") == 0)
                object->pass_authorization = "0";
            else if (strcasecmp(value, "On") == 0)
                object->pass_authorization = "1";
            else
                return "Invalid value for authorization flag.";
        }
        else
            return "Invalid option to WSGI handler script definition.";
    }

    if (cmd->path) {
        WSGIDirectoryConfig *dconfig = NULL;
        dconfig = (WSGIDirectoryConfig *)mconfig;

        if (!dconfig->handler_scripts)
            dconfig->handler_scripts = apr_hash_make(cmd->pool);

        apr_hash_set(dconfig->handler_scripts, name, APR_HASH_KEY_STRING,
                     object);
    }
    else {
        WSGIServerConfig *sconfig = NULL;
        sconfig = ap_get_module_config(cmd->server->module_config,
                                       &wsgi_module);

        if (!sconfig->handler_scripts)
            sconfig->handler_scripts = apr_hash_make(cmd->pool);

        apr_hash_set(sconfig->handler_scripts, name, APR_HASH_KEY_STRING,
                     object);
    }

    return NULL;
}