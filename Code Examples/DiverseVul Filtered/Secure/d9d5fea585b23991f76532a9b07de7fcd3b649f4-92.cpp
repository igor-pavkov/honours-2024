static const char *wsgi_add_import_script(cmd_parms *cmd, void *mconfig,
                                          const char *args)
{
    const char *error = NULL;
    WSGIScriptFile *object = NULL;

    const char *option = NULL;
    const char *value = NULL;

    if (!wsgi_import_list) {
        wsgi_import_list = apr_array_make(cmd->pool, 20,
                                          sizeof(WSGIScriptFile));
    }

    object = (WSGIScriptFile *)apr_array_push(wsgi_import_list);

    object->handler_script = ap_getword_conf(cmd->pool, &args);
    object->process_group = NULL;
    object->application_group = NULL;

    if (!object->handler_script || !*object->handler_script)
        return "Location of import script not supplied.";

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI import script definition.";
        }

        if (!strcmp(option, "application-group")) {
            if (!*value)
                return "Invalid name for WSGI application group.";

            object->application_group = value;
        }
#if defined(MOD_WSGI_WITH_DAEMONS)
        else if (!strcmp(option, "process-group")) {
            if (!*value)
                return "Invalid name for WSGI process group.";

            object->process_group = value;
        }
#endif
        else
            return "Invalid option to WSGI import script definition.";
    }

    if (!object->application_group)
        return "Name of WSGI application group required.";

    if (!strcmp(object->application_group, "%{GLOBAL}"))
        object->application_group = "";

#if defined(MOD_WSGI_WITH_DAEMONS)
    if (!object->process_group)
        return "Name of WSGI process group required.";

    if (!strcmp(object->process_group, "%{GLOBAL}"))
        object->process_group = "";

    if (*object->process_group) {
        WSGIProcessGroup *group = NULL;
        WSGIProcessGroup *entries = NULL;
        WSGIProcessGroup *entry = NULL;
        int i;

        if (!wsgi_daemon_list)
            return "WSGI process group not yet configured.";

        entries = (WSGIProcessGroup *)wsgi_daemon_list->elts;

        for (i = 0; i < wsgi_daemon_list->nelts; ++i) {
            entry = &entries[i];

            if (!strcmp(entry->name, object->process_group)) {
                group = entry;
                break;
            }
        }

        if (!group)
            return "WSGI process group not yet configured.";

        if (group->server != cmd->server && group->server->is_virtual)
            return "WSGI process group not accessible.";
    }
#else
    object->process_group = "";
#endif

    if (!*object->process_group)
        wsgi_python_required = 1;

    return NULL;
}