void manager_send_unit_plymouth(Manager *m, Unit *u) {
        static const union sockaddr_union sa = PLYMOUTH_SOCKET;
        _cleanup_free_ char *message = NULL;
        _cleanup_close_ int fd = -1;
        int n = 0;

        /* Don't generate plymouth events if the service was already
         * started and we're just deserializing */
        if (MANAGER_IS_RELOADING(m))
                return;

        if (!MANAGER_IS_SYSTEM(m))
                return;

        if (detect_container() > 0)
                return;

        if (u->type != UNIT_SERVICE &&
            u->type != UNIT_MOUNT &&
            u->type != UNIT_SWAP)
                return;

        /* We set SOCK_NONBLOCK here so that we rather drop the
         * message then wait for plymouth */
        fd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
        if (fd < 0) {
                log_error_errno(errno, "socket() failed: %m");
                return;
        }

        if (connect(fd, &sa.sa, SOCKADDR_UN_LEN(sa.un)) < 0) {

                if (!IN_SET(errno, EPIPE, EAGAIN, ENOENT, ECONNREFUSED, ECONNRESET, ECONNABORTED))
                        log_error_errno(errno, "connect() failed: %m");
                return;
        }

        if (asprintf(&message, "U\002%c%s%n", (int) (strlen(u->id) + 1), u->id, &n) < 0) {
                log_oom();
                return;
        }

        errno = 0;
        if (write(fd, message, n + 1) != n + 1)
                if (!IN_SET(errno, EPIPE, EAGAIN, ENOENT, ECONNREFUSED, ECONNRESET, ECONNABORTED))
                        log_error_errno(errno, "Failed to write Plymouth message: %m");
}