void unit_log_process_exit(
                Unit *u,
                int level,
                const char *kind,
                const char *command,
                int code,
                int status) {

        assert(u);
        assert(kind);

        if (code != CLD_EXITED)
                level = LOG_WARNING;

        log_struct(level,
                   "MESSAGE_ID=" SD_MESSAGE_UNIT_PROCESS_EXIT_STR,
                   LOG_UNIT_MESSAGE(u, "%s exited, code=%s, status=%i/%s",
                                    kind,
                                    sigchld_code_to_string(code), status,
                                    strna(code == CLD_EXITED
                                          ? exit_status_to_string(status, EXIT_STATUS_FULL)
                                          : signal_to_string(status))),
                   "EXIT_CODE=%s", sigchld_code_to_string(code),
                   "EXIT_STATUS=%i", status,
                   "COMMAND=%s", strna(command),
                   LOG_UNIT_ID(u),
                   LOG_UNIT_INVOCATION_ID(u));
}