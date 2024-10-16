void qmp_guest_shutdown(bool has_mode, const char *mode, Error **err)
{
    const char *shutdown_flag;
    Error *local_err = NULL;
    pid_t pid;
    int status;

    slog("guest-shutdown called, mode: %s", mode);
    if (!has_mode || strcmp(mode, "powerdown") == 0) {
        shutdown_flag = "-P";
    } else if (strcmp(mode, "halt") == 0) {
        shutdown_flag = "-H";
    } else if (strcmp(mode, "reboot") == 0) {
        shutdown_flag = "-r";
    } else {
        error_setg(err,
                   "mode is invalid (valid values are: halt|powerdown|reboot");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* child, start the shutdown */
        setsid();
        reopen_fd_to_null(0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        execle("/sbin/shutdown", "shutdown", shutdown_flag, "+0",
               "hypervisor initiated shutdown", (char*)NULL, environ);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        error_setg_errno(err, errno, "failed to create child process");
        return;
    }

    ga_wait_child(pid, &status, &local_err);
    if (error_is_set(&local_err)) {
        error_propagate(err, local_err);
        return;
    }

    if (!WIFEXITED(status)) {
        error_setg(err, "child process has terminated abnormally");
        return;
    }

    if (WEXITSTATUS(status)) {
        error_setg(err, "child process has failed to shutdown");
        return;
    }

    /* succeeded */
}