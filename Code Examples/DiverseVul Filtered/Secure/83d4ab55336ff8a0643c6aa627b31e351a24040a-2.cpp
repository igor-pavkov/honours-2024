static int append_session_tasks_max(pam_handle_t *handle, sd_bus_message *m, const char *limit) {
        uint64_t val;
        int r;

        /* No need to parse "infinity" here, it will be set unconditionally later in manager_start_scope() */
        if (isempty(limit) || streq(limit, "infinity"))
                return 0;

        r = safe_atou64(limit, &val);
        if (r >= 0) {
                r = sd_bus_message_append(m, "(sv)", "TasksMax", "t", val);
                if (r < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                        return r;
                }
        } else
                pam_syslog(handle, LOG_WARNING, "Failed to parse systemd.limit: %s, ignoring.", limit);

        return 0;
}