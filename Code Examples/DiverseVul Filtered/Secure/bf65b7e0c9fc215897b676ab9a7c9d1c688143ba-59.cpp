int unit_kill_common(
                Unit *u,
                KillWho who,
                int signo,
                pid_t main_pid,
                pid_t control_pid,
                sd_bus_error *error) {

        int r = 0;
        bool killed = false;

        if (IN_SET(who, KILL_MAIN, KILL_MAIN_FAIL)) {
                if (main_pid < 0)
                        return sd_bus_error_setf(error, BUS_ERROR_NO_SUCH_PROCESS, "%s units have no main processes", unit_type_to_string(u->type));
                else if (main_pid == 0)
                        return sd_bus_error_set_const(error, BUS_ERROR_NO_SUCH_PROCESS, "No main process to kill");
        }

        if (IN_SET(who, KILL_CONTROL, KILL_CONTROL_FAIL)) {
                if (control_pid < 0)
                        return sd_bus_error_setf(error, BUS_ERROR_NO_SUCH_PROCESS, "%s units have no control processes", unit_type_to_string(u->type));
                else if (control_pid == 0)
                        return sd_bus_error_set_const(error, BUS_ERROR_NO_SUCH_PROCESS, "No control process to kill");
        }

        if (IN_SET(who, KILL_CONTROL, KILL_CONTROL_FAIL, KILL_ALL, KILL_ALL_FAIL))
                if (control_pid > 0) {
                        if (kill(control_pid, signo) < 0)
                                r = -errno;
                        else
                                killed = true;
                }

        if (IN_SET(who, KILL_MAIN, KILL_MAIN_FAIL, KILL_ALL, KILL_ALL_FAIL))
                if (main_pid > 0) {
                        if (kill(main_pid, signo) < 0)
                                r = -errno;
                        else
                                killed = true;
                }

        if (IN_SET(who, KILL_ALL, KILL_ALL_FAIL) && u->cgroup_path) {
                _cleanup_set_free_ Set *pid_set = NULL;
                int q;

                /* Exclude the main/control pids from being killed via the cgroup */
                pid_set = unit_pid_set(main_pid, control_pid);
                if (!pid_set)
                        return -ENOMEM;

                q = cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path, signo, 0, pid_set, NULL, NULL);
                if (q < 0 && !IN_SET(q, -EAGAIN, -ESRCH, -ENOENT))
                        r = q;
                else
                        killed = true;
        }

        if (r == 0 && !killed && IN_SET(who, KILL_ALL_FAIL, KILL_CONTROL_FAIL))
                return -ESRCH;

        return r;
}