int unit_add_exec_dependencies(Unit *u, ExecContext *c) {
        ExecDirectoryType dt;
        char **dp;
        int r;

        assert(u);
        assert(c);

        if (c->working_directory && !c->working_directory_missing_ok) {
                r = unit_require_mounts_for(u, c->working_directory, UNIT_DEPENDENCY_FILE);
                if (r < 0)
                        return r;
        }

        if (c->root_directory) {
                r = unit_require_mounts_for(u, c->root_directory, UNIT_DEPENDENCY_FILE);
                if (r < 0)
                        return r;
        }

        if (c->root_image) {
                r = unit_require_mounts_for(u, c->root_image, UNIT_DEPENDENCY_FILE);
                if (r < 0)
                        return r;
        }

        for (dt = 0; dt < _EXEC_DIRECTORY_TYPE_MAX; dt++) {
                if (!u->manager->prefix[dt])
                        continue;

                STRV_FOREACH(dp, c->directories[dt].paths) {
                        _cleanup_free_ char *p;

                        p = strjoin(u->manager->prefix[dt], "/", *dp);
                        if (!p)
                                return -ENOMEM;

                        r = unit_require_mounts_for(u, p, UNIT_DEPENDENCY_FILE);
                        if (r < 0)
                                return r;
                }
        }

        if (!MANAGER_IS_SYSTEM(u->manager))
                return 0;

        if (c->private_tmp) {
                const char *p;

                FOREACH_STRING(p, "/tmp", "/var/tmp") {
                        r = unit_require_mounts_for(u, p, UNIT_DEPENDENCY_FILE);
                        if (r < 0)
                                return r;
                }

                r = unit_add_dependency_by_name(u, UNIT_AFTER, SPECIAL_TMPFILES_SETUP_SERVICE, true, UNIT_DEPENDENCY_FILE);
                if (r < 0)
                        return r;
        }

        if (!IN_SET(c->std_output,
                    EXEC_OUTPUT_JOURNAL, EXEC_OUTPUT_JOURNAL_AND_CONSOLE,
                    EXEC_OUTPUT_KMSG, EXEC_OUTPUT_KMSG_AND_CONSOLE,
                    EXEC_OUTPUT_SYSLOG, EXEC_OUTPUT_SYSLOG_AND_CONSOLE) &&
            !IN_SET(c->std_error,
                    EXEC_OUTPUT_JOURNAL, EXEC_OUTPUT_JOURNAL_AND_CONSOLE,
                    EXEC_OUTPUT_KMSG, EXEC_OUTPUT_KMSG_AND_CONSOLE,
                    EXEC_OUTPUT_SYSLOG, EXEC_OUTPUT_SYSLOG_AND_CONSOLE))
                return 0;

        /* If syslog or kernel logging is requested, make sure our own
         * logging daemon is run first. */

        r = unit_add_dependency_by_name(u, UNIT_AFTER, SPECIAL_JOURNALD_SOCKET, true, UNIT_DEPENDENCY_FILE);
        if (r < 0)
                return r;

        return 0;
}