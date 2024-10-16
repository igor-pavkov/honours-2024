static int manager_dispatch_ask_password_fd(sd_event_source *source,
                                            int fd, uint32_t revents, void *userdata) {
        Manager *m = userdata;

        assert(m);

        flush_fd(fd);

        m->have_ask_password = have_ask_password();
        if (m->have_ask_password < 0)
                /* Log error but continue. Negative have_ask_password
                 * is treated as unknown status. */
                log_error_errno(m->have_ask_password, "Failed to list /run/systemd/ask-password: %m");

        return 0;
}