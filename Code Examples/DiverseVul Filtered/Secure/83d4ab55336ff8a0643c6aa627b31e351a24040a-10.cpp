static int append_session_cg_weight(pam_handle_t *handle, sd_bus_message *m, const char *limit, const char *field) {
        uint64_t val;
        int r;

        if (isempty(limit))
                return 0;

        r = cg_weight_parse(limit, &val);
        if (r >= 0) {
                r = sd_bus_message_append(m, "(sv)", field, "t", val);
                if (r < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                        return r;
                }
        } else if (streq(field, "CPUWeight"))
                pam_syslog(handle, LOG_WARNING, "Failed to parse systemd.cpu_weight: %s, ignoring.", limit);
        else
                pam_syslog(handle, LOG_WARNING, "Failed to parse systemd.io_weight: %s, ignoring.", limit);

        return 0;
}