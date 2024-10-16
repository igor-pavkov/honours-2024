static int log_leftover(pid_t pid, int sig, void *userdata) {
        _cleanup_free_ char *comm = NULL;

        (void) get_process_comm(pid, &comm);

        if (comm && comm[0] == '(') /* Most likely our own helper process (PAM?), ignore */
                return 0;

        log_unit_warning(userdata,
                         "Found left-over process " PID_FMT " (%s) in control group while starting unit. Ignoring.\n"
                         "This usually indicates unclean termination of a previous run, or service implementation deficiencies.",
                         pid, strna(comm));

        return 1;
}