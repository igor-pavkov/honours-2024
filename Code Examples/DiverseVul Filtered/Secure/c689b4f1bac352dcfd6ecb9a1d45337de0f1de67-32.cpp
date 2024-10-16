void qmp_guest_set_time(int64_t time_ns, Error **errp)
{
    int ret;
    int status;
    pid_t pid;
    Error *local_err = NULL;
    struct timeval tv;

    /* year-2038 will overflow in case time_t is 32bit */
    if (time_ns / 1000000000 != (time_t)(time_ns / 1000000000)) {
        error_setg(errp, "Time %" PRId64 " is too large", time_ns);
        return;
    }

    tv.tv_sec = time_ns / 1000000000;
    tv.tv_usec = (time_ns % 1000000000) / 1000;

    ret = settimeofday(&tv, NULL);
    if (ret < 0) {
        error_setg_errno(errp, errno, "Failed to set time to guest");
        return;
    }

    /* Set the Hardware Clock to the current System Time. */
    pid = fork();
    if (pid == 0) {
        setsid();
        reopen_fd_to_null(0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        execle("/sbin/hwclock", "hwclock", "-w", NULL, environ);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        error_setg_errno(errp, errno, "failed to create child process");
        return;
    }

    ga_wait_child(pid, &status, &local_err);
    if (error_is_set(&local_err)) {
        error_propagate(errp, local_err);
        return;
    }

    if (!WIFEXITED(status)) {
        error_setg(errp, "child process has terminated abnormally");
        return;
    }

    if (WEXITSTATUS(status)) {
        error_setg(errp, "hwclock failed to set hardware clock to system time");
        return;
    }
}