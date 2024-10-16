int server_open_native_socket(Server *s) {

        static const union sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
                .un.sun_path = "/run/systemd/journal/socket",
        };
        int r;

        assert(s);

        if (s->native_fd < 0) {
                s->native_fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
                if (s->native_fd < 0)
                        return log_error_errno(errno, "socket() failed: %m");

                (void) sockaddr_un_unlink(&sa.un);

                r = bind(s->native_fd, &sa.sa, SOCKADDR_UN_LEN(sa.un));
                if (r < 0)
                        return log_error_errno(errno, "bind(%s) failed: %m", sa.un.sun_path);

                (void) chmod(sa.un.sun_path, 0666);
        } else
                (void) fd_nonblock(s->native_fd, true);

        r = setsockopt_int(s->native_fd, SOL_SOCKET, SO_PASSCRED, true);
        if (r < 0)
                return log_error_errno(r, "SO_PASSCRED failed: %m");

#if HAVE_SELINUX
        if (mac_selinux_use()) {
                r = setsockopt_int(s->native_fd, SOL_SOCKET, SO_PASSSEC, true);
                if (r < 0)
                        log_warning_errno(r, "SO_PASSSEC failed: %m");
        }
#endif

        r = setsockopt_int(s->native_fd, SOL_SOCKET, SO_TIMESTAMP, true);
        if (r < 0)
                return log_error_errno(r, "SO_TIMESTAMP failed: %m");

        r = sd_event_add_io(s->event, &s->native_event_source, s->native_fd, EPOLLIN, server_process_datagram, s);
        if (r < 0)
                return log_error_errno(r, "Failed to add native server fd to event loop: %m");

        r = sd_event_source_set_priority(s->native_event_source, SD_EVENT_PRIORITY_NORMAL+5);
        if (r < 0)
                return log_error_errno(r, "Failed to adjust native event source priority: %m");

        return 0;
}