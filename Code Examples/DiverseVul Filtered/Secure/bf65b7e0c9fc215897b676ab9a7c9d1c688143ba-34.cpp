int unit_kill_context(
                Unit *u,
                KillContext *c,
                KillOperation k,
                pid_t main_pid,
                pid_t control_pid,
                bool main_pid_alien) {

        bool wait_for_exit = false, send_sighup;
        cg_kill_log_func_t log_func = NULL;
        int sig, r;

        assert(u);
        assert(c);

        /* Kill the processes belonging to this unit, in preparation for shutting the unit down.
         * Returns > 0 if we killed something worth waiting for, 0 otherwise. */

        if (c->kill_mode == KILL_NONE)
                return 0;

        sig = operation_to_signal(c, k);

        send_sighup =
                c->send_sighup &&
                IN_SET(k, KILL_TERMINATE, KILL_TERMINATE_AND_LOG) &&
                sig != SIGHUP;

        if (k != KILL_TERMINATE || IN_SET(sig, SIGKILL, SIGABRT))
                log_func = log_kill;

        if (main_pid > 0) {
                if (log_func)
                        log_func(main_pid, sig, u);

                r = kill_and_sigcont(main_pid, sig);
                if (r < 0 && r != -ESRCH) {
                        _cleanup_free_ char *comm = NULL;
                        (void) get_process_comm(main_pid, &comm);

                        log_unit_warning_errno(u, r, "Failed to kill main process " PID_FMT " (%s), ignoring: %m", main_pid, strna(comm));
                } else {
                        if (!main_pid_alien)
                                wait_for_exit = true;

                        if (r != -ESRCH && send_sighup)
                                (void) kill(main_pid, SIGHUP);
                }
        }

        if (control_pid > 0) {
                if (log_func)
                        log_func(control_pid, sig, u);

                r = kill_and_sigcont(control_pid, sig);
                if (r < 0 && r != -ESRCH) {
                        _cleanup_free_ char *comm = NULL;
                        (void) get_process_comm(control_pid, &comm);

                        log_unit_warning_errno(u, r, "Failed to kill control process " PID_FMT " (%s), ignoring: %m", control_pid, strna(comm));
                } else {
                        wait_for_exit = true;

                        if (r != -ESRCH && send_sighup)
                                (void) kill(control_pid, SIGHUP);
                }
        }

        if (u->cgroup_path &&
            (c->kill_mode == KILL_CONTROL_GROUP || (c->kill_mode == KILL_MIXED && k == KILL_KILL))) {
                _cleanup_set_free_ Set *pid_set = NULL;

                /* Exclude the main/control pids from being killed via the cgroup */
                pid_set = unit_pid_set(main_pid, control_pid);
                if (!pid_set)
                        return -ENOMEM;

                r = cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path,
                                      sig,
                                      CGROUP_SIGCONT|CGROUP_IGNORE_SELF,
                                      pid_set,
                                      log_func, u);
                if (r < 0) {
                        if (!IN_SET(r, -EAGAIN, -ESRCH, -ENOENT))
                                log_unit_warning_errno(u, r, "Failed to kill control group %s, ignoring: %m", u->cgroup_path);

                } else if (r > 0) {

                        /* FIXME: For now, on the legacy hierarchy, we will not wait for the cgroup members to die if
                         * we are running in a container or if this is a delegation unit, simply because cgroup
                         * notification is unreliable in these cases. It doesn't work at all in containers, and outside
                         * of containers it can be confused easily by left-over directories in the cgroup — which
                         * however should not exist in non-delegated units. On the unified hierarchy that's different,
                         * there we get proper events. Hence rely on them. */

                        if (cg_unified_controller(SYSTEMD_CGROUP_CONTROLLER) > 0 ||
                            (detect_container() == 0 && !unit_cgroup_delegate(u)))
                                wait_for_exit = true;

                        if (send_sighup) {
                                set_free(pid_set);

                                pid_set = unit_pid_set(main_pid, control_pid);
                                if (!pid_set)
                                        return -ENOMEM;

                                cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path,
                                                  SIGHUP,
                                                  CGROUP_IGNORE_SELF,
                                                  pid_set,
                                                  NULL, NULL);
                        }
                }
        }

        return wait_for_exit;
}