static void server_process_entry_meta(
                const char *p, size_t l,
                const struct ucred *ucred,
                int *priority,
                char **identifier,
                char **message,
                pid_t *object_pid) {

        /* We need to determine the priority of this entry for the rate limiting logic */

        if (l == 10 &&
            startswith(p, "PRIORITY=") &&
            p[9] >= '0' && p[9] <= '9')
                *priority = (*priority & LOG_FACMASK) | (p[9] - '0');

        else if (l == 17 &&
                 startswith(p, "SYSLOG_FACILITY=") &&
                 p[16] >= '0' && p[16] <= '9')
                *priority = (*priority & LOG_PRIMASK) | ((p[16] - '0') << 3);

        else if (l == 18 &&
                 startswith(p, "SYSLOG_FACILITY=") &&
                 p[16] >= '0' && p[16] <= '9' &&
                 p[17] >= '0' && p[17] <= '9')
                *priority = (*priority & LOG_PRIMASK) | (((p[16] - '0')*10 + (p[17] - '0')) << 3);

        else if (l >= 19 &&
                 startswith(p, "SYSLOG_IDENTIFIER=")) {
                char *t;

                t = strndup(p + 18, l - 18);
                if (t) {
                        free(*identifier);
                        *identifier = t;
                }

        } else if (l >= 8 &&
                   startswith(p, "MESSAGE=")) {
                char *t;

                t = strndup(p + 8, l - 8);
                if (t) {
                        free(*message);
                        *message = t;
                }

        } else if (l > STRLEN("OBJECT_PID=") &&
                   l < STRLEN("OBJECT_PID=")  + DECIMAL_STR_MAX(pid_t) &&
                   startswith(p, "OBJECT_PID=") &&
                   allow_object_pid(ucred)) {
                char buf[DECIMAL_STR_MAX(pid_t)];
                memcpy(buf, p + STRLEN("OBJECT_PID="),
                       l - STRLEN("OBJECT_PID="));
                buf[l-STRLEN("OBJECT_PID=")] = '\0';

                (void) parse_pid(buf, object_pid);
        }
}