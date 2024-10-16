static int append_session_memory_max(pam_handle_t *handle, sd_bus_message *m, const char *limit) {
        uint64_t val;
        int r;

        if (isempty(limit))
                return 0;

        if (streq(limit, "infinity")) {
                r = sd_bus_message_append(m, "(sv)", "MemoryMax", "t", (uint64_t)-1);
                if (r < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                        return r;
                }
        } else {
                r = parse_permille(limit);
                if (r >= 0) {
                        r = sd_bus_message_append(m, "(sv)", "MemoryMaxScale", "u", (uint32_t) (((uint64_t) r * UINT32_MAX) / 1000U));
                        if (r < 0) {
                                pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                                return r;
                        }
                } else {
                        r = parse_size(limit, 1024, &val);
                        if (r >= 0) {
                                r = sd_bus_message_append(m, "(sv)", "MemoryMax", "t", val);
                                if (r < 0) {
                                        pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                                        return r;
                                }
                        } else
                                pam_syslog(handle, LOG_WARNING, "Failed to parse systemd.limit: %s, ignoring.", limit);
                }
        }

        return 0;
}