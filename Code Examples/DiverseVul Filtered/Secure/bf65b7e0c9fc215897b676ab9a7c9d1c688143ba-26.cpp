int unit_fork_helper_process(Unit *u, const char *name, pid_t *ret) {
        int r;

        assert(u);
        assert(ret);

        /* Forks off a helper process and makes sure it is a member of the unit's cgroup. Returns == 0 in the child,
         * and > 0 in the parent. The pid parameter is always filled in with the child's PID. */

        (void) unit_realize_cgroup(u);

        r = safe_fork(name, FORK_REOPEN_LOG, ret);
        if (r != 0)
                return r;

        (void) default_signals(SIGNALS_CRASH_HANDLER, SIGNALS_IGNORE, -1);
        (void) ignore_signals(SIGPIPE, -1);

        (void) prctl(PR_SET_PDEATHSIG, SIGTERM);

        if (u->cgroup_path) {
                r = cg_attach_everywhere(u->manager->cgroup_supported, u->cgroup_path, 0, NULL, NULL);
                if (r < 0) {
                        log_unit_error_errno(u, r, "Failed to join unit cgroup %s: %m", u->cgroup_path);
                        _exit(EXIT_CGROUP);
                }
        }

        return 0;
}