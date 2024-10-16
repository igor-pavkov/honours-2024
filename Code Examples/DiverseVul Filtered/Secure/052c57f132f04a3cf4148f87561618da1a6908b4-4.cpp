void server_process_native_message(
                Server *s,
                const char *buffer, size_t buffer_size,
                const struct ucred *ucred,
                const struct timeval *tv,
                const char *label, size_t label_len) {

        size_t remaining = buffer_size;
        ClientContext *context = NULL;
        int r;

        assert(s);
        assert(buffer || buffer_size == 0);

        if (ucred && pid_is_valid(ucred->pid)) {
                r = client_context_get(s, ucred->pid, ucred, label, label_len, NULL, &context);
                if (r < 0)
                        log_warning_errno(r, "Failed to retrieve credentials for PID " PID_FMT ", ignoring: %m", ucred->pid);
        }

        do {
                r = server_process_entry(s,
                                         (const uint8_t*) buffer + (buffer_size - remaining), &remaining,
                                         context, ucred, tv, label, label_len);
        } while (r == 0);
}