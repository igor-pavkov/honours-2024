static const char *wsgi_add_daemon_process(cmd_parms *cmd, void *mconfig,
                                           const char *args)
{
    const char *name = NULL;
    const char *user = NULL;
    const char *group = NULL;

    int processes = 1;
    int multiprocess = 0;
    int threads = 15;
    int umask = -1;

    const char *root = NULL;
    const char *home = NULL;

    const char *lang = NULL;
    const char *locale = NULL;

    const char *python_home = NULL;
    const char *python_path = NULL;
    const char *python_eggs = NULL;

    int stack_size = 0;
    int maximum_requests = 0;
    int shutdown_timeout = 5;
    int deadlock_timeout = 300;
    int inactivity_timeout = 0;

    const char *display_name = NULL;

    int send_buffer_size = 0;
    int recv_buffer_size = 0;

    const char *script_user = NULL;
    const char *script_group = NULL;

    int cpu_time_limit = 0;
    int cpu_priority = 0;

    apr_int64_t memory_limit = 0;
    apr_int64_t virtual_memory_limit = 0;

    uid_t uid;
    uid_t gid;

    const char *groups_list = NULL;
    int groups_count = 0;
    gid_t *groups = NULL;

    const char *option = NULL;
    const char *value = NULL;

    WSGIProcessGroup *entries = NULL;
    WSGIProcessGroup *entry = NULL;

    int i;

    /*
     * Set the defaults for user/group from values
     * defined for the User/Group directives in main
     * Apache configuration.
     */

    uid = ap_unixd_config.user_id;
    user = ap_unixd_config.user_name;

    gid = ap_unixd_config.group_id;

    /* Now parse options for directive. */

    name = ap_getword_conf(cmd->pool, &args);

    if (!name || !*name)
        return "Name of WSGI daemon process not supplied.";

    while (*args) {
        if (wsgi_parse_option(cmd->pool, &args, &option,
                              &value) != APR_SUCCESS) {
            return "Invalid option to WSGI daemon process definition.";
        }

        if (!strcmp(option, "user")) {
            if (!*value)
                return "Invalid user for WSGI daemon process.";

            user = value;
            uid = ap_uname2id(user);
            if (uid == 0)
                return "WSGI process blocked from running as root.";

            if (*user == '#') {
                struct passwd *entry = NULL;

                if ((entry = getpwuid(uid)) == NULL)
                    return "Couldn't determine user name from uid.";

                user = entry->pw_name;
            }
        }
        else if (!strcmp(option, "group")) {
            if (!*value)
                return "Invalid group for WSGI daemon process.";

            group = value;
            gid = ap_gname2id(group);
        }
        else if (!strcmp(option, "supplementary-groups")) {
            groups_list = value;
        }
        else if (!strcmp(option, "processes")) {
            if (!*value)
                return "Invalid process count for WSGI daemon process.";

            processes = atoi(value);
            if (processes < 1)
                return "Invalid process count for WSGI daemon process.";

            multiprocess = 1;
        }
        else if (!strcmp(option, "threads")) {
            if (!*value)
                return "Invalid thread count for WSGI daemon process.";

            threads = atoi(value);
            if (threads < 1 || threads >= WSGI_STACK_LAST-1)
                return "Invalid thread count for WSGI daemon process.";
        }
        else if (!strcmp(option, "umask")) {
            if (!*value)
                return "Invalid umask for WSGI daemon process.";

            errno = 0;
            umask = strtol(value, (char **)&value, 8);

            if (*value || errno == ERANGE || umask < 0)
                return "Invalid umask for WSGI daemon process.";
        }
        else if (!strcmp(option, "chroot")) {
            if (geteuid())
                return "Cannot chroot WSGI daemon process when not root.";

            if (*value != '/')
                return "Invalid chroot directory for WSGI daemon process.";

            root = value;
        }
        else if (!strcmp(option, "home")) {
            if (*value != '/')
                return "Invalid home directory for WSGI daemon process.";

            home = value;
        }
        else if (!strcmp(option, "lang")) {
            lang = value;
        }
        else if (!strcmp(option, "locale")) {
            locale = value;
        }
        else if (!strcmp(option, "python-home")) {
            python_home = value;
        }
        else if (!strcmp(option, "python-path")) {
            python_path = value;
        }
        else if (!strcmp(option, "python-eggs")) {
            python_eggs = value;
        }
#if (APR_MAJOR_VERSION >= 1)
        else if (!strcmp(option, "stack-size")) {
            if (!*value)
                return "Invalid stack size for WSGI daemon process.";

            stack_size = atoi(value);
            if (stack_size <= 0)
                return "Invalid stack size for WSGI daemon process.";
        }
#endif
        else if (!strcmp(option, "maximum-requests")) {
            if (!*value)
                return "Invalid request count for WSGI daemon process.";

            maximum_requests = atoi(value);
            if (maximum_requests < 0)
                return "Invalid request count for WSGI daemon process.";
        }
        else if (!strcmp(option, "shutdown-timeout")) {
            if (!*value)
                return "Invalid shutdown timeout for WSGI daemon process.";

            shutdown_timeout = atoi(value);
            if (shutdown_timeout < 0)
                return "Invalid shutdown timeout for WSGI daemon process.";
        }
        else if (!strcmp(option, "deadlock-timeout")) {
            if (!*value)
                return "Invalid deadlock timeout for WSGI daemon process.";

            deadlock_timeout = atoi(value);
            if (deadlock_timeout < 0)
                return "Invalid deadlock timeout for WSGI daemon process.";
        }
        else if (!strcmp(option, "inactivity-timeout")) {
            if (!*value)
                return "Invalid inactivity timeout for WSGI daemon process.";

            inactivity_timeout = atoi(value);
            if (inactivity_timeout < 0)
                return "Invalid inactivity timeout for WSGI daemon process.";
        }
        else if (!strcmp(option, "display-name")) {
            display_name = value;
        }
        else if (!strcmp(option, "send-buffer-size")) {
            if (!*value)
                return "Invalid send buffer size for WSGI daemon process.";

            send_buffer_size = atoi(value);
            if (send_buffer_size < 512 && send_buffer_size != 0) {
                return "Send buffer size must be >= 512 bytes, "
                       "or 0 for system default.";
            }
        }
        else if (!strcmp(option, "receive-buffer-size")) {
            if (!*value)
                return "Invalid receive buffer size for WSGI daemon process.";

            recv_buffer_size = atoi(value);
            if (recv_buffer_size < 512 && recv_buffer_size != 0) {
                return "Receive buffer size must be >= 512 bytes, "
                       "or 0 for system default.";
            }
        }
        else if (!strcmp(option, "script-user")) {
            uid_t script_uid;

            if (!*value)
                return "Invalid script user for WSGI daemon process.";

            script_uid = ap_uname2id(value);

            if (*value == '#') {
                struct passwd *entry = NULL;

                if ((entry = getpwuid(script_uid)) == NULL)
                    return "Couldn't determine uid from script user.";

                value = entry->pw_name;
            }

            script_user = value;
        }
        else if (!strcmp(option, "script-group")) {
            gid_t script_gid;

            if (!*value)
                return "Invalid script group for WSGI daemon process.";

            script_gid = ap_gname2id(value);

            if (*value == '#') {
                struct group *entry = NULL;

                if ((entry = getgrgid(script_gid)) == NULL)
                    return "Couldn't determine gid from script group.";

                value = entry->gr_name;
            }

            script_group = value;
        }
        else if (!strcmp(option, "cpu-time-limit")) {
            if (!*value)
                return "Invalid CPU time limit for WSGI daemon process.";

            cpu_time_limit = atoi(value);
            if (cpu_time_limit < 0)
                return "Invalid CPU time limit for WSGI daemon process.";
        }
        else if (!strcmp(option, "cpu-priority")) {
            if (!*value)
                return "Invalid CPU priority for WSGI daemon process.";

            cpu_priority = atoi(value);
        }
        else if (!strcmp(option, "memory-limit")) {
            if (!*value)
                return "Invalid memory limit for WSGI daemon process.";

            memory_limit = apr_atoi64(value);
            if (memory_limit < 0)
                return "Invalid memory limit for WSGI daemon process.";
        }
        else if (!strcmp(option, "virtual-memory-limit")) {
            if (!*value)
                return "Invalid virtual memory limit for WSGI daemon process.";

            virtual_memory_limit = apr_atoi64(value);
            if (virtual_memory_limit < 0)
                return "Invalid virtual memory limit for WSGI daemon process.";
        }
        else
            return "Invalid option to WSGI daemon process definition.";
    }

    if (script_user && script_group)
        return "Only one of script-user and script-group allowed.";

    if (groups_list) {
        const char *group_name = NULL;
        int groups_maximum = NGROUPS_MAX;
        const char *items = NULL;

#ifdef _SC_NGROUPS_MAX
        groups_maximum = sysconf(_SC_NGROUPS_MAX);
        if (groups_maximum < 0)
            groups_maximum = NGROUPS_MAX;
#endif
        groups = (gid_t *)apr_pcalloc(cmd->pool,
                                      groups_maximum*sizeof(groups[0]));

        groups[groups_count++] = gid;

        items = groups_list;
        group_name = ap_getword(cmd->pool, &items, ',');

        while (group_name && *group_name) {
            if (groups_count > groups_maximum)
                return "Too many supplementary groups WSGI daemon process";

            groups[groups_count++] = ap_gname2id(group_name);
            group_name = ap_getword(cmd->pool, &items, ',');
        }
    }

    if (!wsgi_daemon_list) {
        wsgi_daemon_list = apr_array_make(cmd->pool, 20,
                                          sizeof(WSGIProcessGroup));
    }

    entries = (WSGIProcessGroup *)wsgi_daemon_list->elts;

    for (i = 0; i < wsgi_daemon_list->nelts; ++i) {
        entry = &entries[i];

        if (!strcmp(entry->name, name))
            return "Name duplicates previous WSGI daemon definition.";
    }

    wsgi_daemon_count++;

    entry = (WSGIProcessGroup *)apr_array_push(wsgi_daemon_list);

    entry->server = cmd->server;

    entry->random = random();
    entry->id = wsgi_daemon_count;

    entry->name = apr_pstrdup(cmd->pool, name);
    entry->user = apr_pstrdup(cmd->pool, user);
    entry->group = apr_pstrdup(cmd->pool, group);

    entry->uid = uid;
    entry->gid = gid;

    entry->groups_list = groups_list;
    entry->groups_count = groups_count;
    entry->groups = groups;

    entry->processes = processes;
    entry->multiprocess = multiprocess;
    entry->threads = threads;

    entry->umask = umask;
    entry->root = root;
    entry->home = home;

    entry->lang = lang;
    entry->locale = locale;

    entry->python_home = python_home;
    entry->python_path = python_path;
    entry->python_eggs = python_eggs;

    entry->stack_size = stack_size;
    entry->maximum_requests = maximum_requests;
    entry->shutdown_timeout = shutdown_timeout;
    entry->deadlock_timeout = apr_time_from_sec(deadlock_timeout);
    entry->inactivity_timeout = apr_time_from_sec(inactivity_timeout);

    entry->display_name = display_name;

    entry->send_buffer_size = send_buffer_size;
    entry->recv_buffer_size = recv_buffer_size;

    entry->script_user = script_user;
    entry->script_group = script_group;

    entry->cpu_time_limit = cpu_time_limit;
    entry->cpu_priority = cpu_priority;

    entry->memory_limit = memory_limit;
    entry->virtual_memory_limit = virtual_memory_limit;

    entry->listener_fd = -1;

    return NULL;
}