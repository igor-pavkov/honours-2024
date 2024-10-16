int manager_dispatch_user_lookup_fd(sd_event_source *source, int fd, uint32_t revents, void *userdata) {
        struct buffer {
                uid_t uid;
                gid_t gid;
                char unit_name[UNIT_NAME_MAX+1];
        } _packed_ buffer;

        Manager *m = userdata;
        ssize_t l;
        size_t n;
        Unit *u;

        assert_se(source);
        assert_se(m);

        /* Invoked whenever a child process succeeded resolving its user/group to use and sent us the resulting UID/GID
         * in a datagram. We parse the datagram here and pass it off to the unit, so that it can add a reference to the
         * UID/GID so that it can destroy the UID/GID's IPC objects when the reference counter drops to 0. */

        l = recv(fd, &buffer, sizeof(buffer), MSG_DONTWAIT);
        if (l < 0) {
                if (errno == EINTR || errno == EAGAIN)
                        return 0;

                return log_error_errno(errno, "Failed to read from user lookup fd: %m");
        }

        if ((size_t) l <= offsetof(struct buffer, unit_name)) {
                log_warning("Received too short user lookup message, ignoring.");
                return 0;
        }

        if ((size_t) l > offsetof(struct buffer, unit_name) + UNIT_NAME_MAX) {
                log_warning("Received too long user lookup message, ignoring.");
                return 0;
        }

        if (!uid_is_valid(buffer.uid) && !gid_is_valid(buffer.gid)) {
                log_warning("Got user lookup message with invalid UID/GID pair, ignoring.");
                return 0;
        }

        n = (size_t) l - offsetof(struct buffer, unit_name);
        if (memchr(buffer.unit_name, 0, n)) {
                log_warning("Received lookup message with embedded NUL character, ignoring.");
                return 0;
        }

        buffer.unit_name[n] = 0;
        u = manager_get_unit(m, buffer.unit_name);
        if (!u) {
                log_debug("Got user lookup message but unit doesn't exist, ignoring.");
                return 0;
        }

        log_unit_debug(u, "User lookup succeeded: uid=" UID_FMT " gid=" GID_FMT, buffer.uid, buffer.gid);

        unit_notify_user_lookup(u, buffer.uid, buffer.gid);
        return 0;
}