static int log_kill(pid_t pid, int sig, void *userdata) {
        _cleanup_free_ char *comm = NULL;

        (void) get_process_comm(pid, &comm);

        /* Don't log about processes marked with brackets, under the assumption that these are temporary processes
           only, like for example systemd's own PAM stub process. */
        if (comm && comm[0] == '(')
                return 0;

        log_unit_notice(userdata,
                        "Killing process " PID_FMT " (%s) with signal SIG%s.",
                        pid,
                        strna(comm),
                        signal_to_string(sig));

        return 1;
}