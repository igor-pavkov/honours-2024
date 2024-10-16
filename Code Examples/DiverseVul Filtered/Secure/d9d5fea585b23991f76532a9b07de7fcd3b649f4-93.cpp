static const char *wsgi_add_script_alias(cmd_parms *cmd, void *mconfig,
                                         const char *args)
{
    const char *l = NULL;
    const char *a = NULL;

    WSGIServerConfig *sconfig = NULL;
    WSGIAliasEntry *entry = NULL;

    const char *option = NULL;
    const char *value = NULL;

#if defined(MOD_WSGI_WITH_DAEMONS)
    const char *process_group = NULL;
#else
    const char *process_group = "";
#endif

    const char *application_group = NULL;
    const char *callable_object = NULL;

    int pass_authorization = -1;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

    if (!sconfig->alias_list) {
        sconfig->alias_list = apr_array_make(sconfig->pool, 20,
                                            sizeof(WSGIAliasEntry));
    }

    l = ap_getword_conf(cmd->pool, &args);

    if (*l == '\0' || *args == 0) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name,
                           " requires at least two arguments",
                           cmd->cmd->errmsg ? ", " : NULL,
                           cmd->cmd->errmsg, NULL);
    }

    a = ap_getword_conf(cmd->pool, &args);

    if (*a == '\0') {
        return apr_pstrcat(cmd->pool, cmd->cmd->name,
                           " requires at least two arguments",
                           cmd->cmd->errmsg ? ", " : NULL,
                           cmd->cmd->errmsg, NULL);
    }

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI script alias definition.";
        }

        if (!cmd->info && !strcmp(option, "application-group")) {
            if (!*value)
                return "Invalid name for WSGI application group.";

            if (!strcmp(value, "%{GLOBAL}"))
                value = "";

            application_group = value;
        }
#if defined(MOD_WSGI_WITH_DAEMONS)
        else if (!cmd->info && !strcmp(option, "process-group")) {
            if (!*value)
                return "Invalid name for WSGI process group.";

            if (!strcmp(value, "%{GLOBAL}"))
                value = "";

            process_group = value;
        }
#endif
        else if (!strcmp(option, "callable-object")) {
            if (!*value)
                return "Invalid name for WSGI callable object.";

            callable_object = value;
        }
        else if (!strcmp(option, "pass-authorization")) {
            if (!*value)
                return "Invalid value for authorization flag.";

            if (strcasecmp(value, "Off") == 0)
                pass_authorization = 0;
            else if (strcasecmp(value, "On") == 0)
                pass_authorization = 1;
            else
                return "Invalid value for authorization flag.";
        }
        else
            return "Invalid option to WSGI script alias definition.";
    }

    entry = (WSGIAliasEntry *)apr_array_push(sconfig->alias_list);

    if (cmd->info) {
        entry->regexp = ap_pregcomp(cmd->pool, l, AP_REG_EXTENDED);
        if (!entry->regexp)
            return "Regular expression could not be compiled.";
    }

    entry->location = l;
    entry->application = a;

    entry->process_group = process_group;
    entry->application_group = application_group;
    entry->callable_object = callable_object;
    entry->pass_authorization = pass_authorization;

    if (process_group && application_group &&
        !strstr(process_group, "%{") &&
        !strstr(application_group, "%{")) {

        WSGIScriptFile *object = NULL;

        if (!wsgi_import_list) {
            wsgi_import_list = apr_array_make(sconfig->pool, 20,
                                              sizeof(WSGIScriptFile));
        }

        object = (WSGIScriptFile *)apr_array_push(wsgi_import_list);

        object->handler_script = a;
        object->process_group = process_group;
        object->application_group = application_group;

#if defined(MOD_WSGI_WITH_DAEMONS)
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
#endif
    }

    return NULL;
}