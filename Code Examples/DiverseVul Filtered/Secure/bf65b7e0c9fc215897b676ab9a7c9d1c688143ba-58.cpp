static Set *unit_pid_set(pid_t main_pid, pid_t control_pid) {
        _cleanup_set_free_ Set *pid_set = NULL;
        int r;

        pid_set = set_new(NULL);
        if (!pid_set)
                return NULL;

        /* Exclude the main/control pids from being killed via the cgroup */
        if (main_pid > 0) {
                r = set_put(pid_set, PID_TO_PTR(main_pid));
                if (r < 0)
                        return NULL;
        }

        if (control_pid > 0) {
                r = set_put(pid_set, PID_TO_PTR(control_pid));
                if (r < 0)
                        return NULL;
        }

        return TAKE_PTR(pid_set);
}