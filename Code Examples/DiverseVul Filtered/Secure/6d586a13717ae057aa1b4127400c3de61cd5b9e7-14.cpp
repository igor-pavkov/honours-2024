int bus_socket_read_message(sd_bus *bus) {
        struct msghdr mh;
        struct iovec iov = {};
        ssize_t k;
        size_t need;
        int r;
        void *b;
        union {
                struct cmsghdr cmsghdr;
                uint8_t buf[CMSG_SPACE(sizeof(int) * BUS_FDS_MAX)];
        } control;
        bool handle_cmsg = false;

        assert(bus);
        assert(IN_SET(bus->state, BUS_RUNNING, BUS_HELLO));

        r = bus_socket_read_message_need(bus, &need);
        if (r < 0)
                return r;

        if (bus->rbuffer_size >= need)
                return bus_socket_make_message(bus, need);

        b = realloc(bus->rbuffer, need);
        if (!b)
                return -ENOMEM;

        bus->rbuffer = b;

        iov = IOVEC_MAKE((uint8_t *)bus->rbuffer + bus->rbuffer_size, need - bus->rbuffer_size);

        if (bus->prefer_readv)
                k = readv(bus->input_fd, &iov, 1);
        else {
                zero(mh);
                mh.msg_iov = &iov;
                mh.msg_iovlen = 1;
                mh.msg_control = &control;
                mh.msg_controllen = sizeof(control);

                k = recvmsg(bus->input_fd, &mh, MSG_DONTWAIT|MSG_CMSG_CLOEXEC);
                if (k < 0 && errno == ENOTSOCK) {
                        bus->prefer_readv = true;
                        k = readv(bus->input_fd, &iov, 1);
                } else
                        handle_cmsg = true;
        }
        if (k < 0)
                return errno == EAGAIN ? 0 : -errno;
        if (k == 0)
                return -ECONNRESET;

        bus->rbuffer_size += k;

        if (handle_cmsg) {
                struct cmsghdr *cmsg;

                CMSG_FOREACH(cmsg, &mh)
                        if (cmsg->cmsg_level == SOL_SOCKET &&
                            cmsg->cmsg_type == SCM_RIGHTS) {
                                int n, *f, i;

                                n = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);

                                if (!bus->can_fds) {
                                        /* Whut? We received fds but this
                                         * isn't actually enabled? Close them,
                                         * and fail */

                                        close_many((int*) CMSG_DATA(cmsg), n);
                                        return -EIO;
                                }

                                f = reallocarray(bus->fds, bus->n_fds + n, sizeof(int));
                                if (!f) {
                                        close_many((int*) CMSG_DATA(cmsg), n);
                                        return -ENOMEM;
                                }

                                for (i = 0; i < n; i++)
                                        f[bus->n_fds++] = fd_move_above_stdio(((int*) CMSG_DATA(cmsg))[i]);
                                bus->fds = f;
                        } else
                                log_debug("Got unexpected auxiliary data with level=%d and type=%d",
                                          cmsg->cmsg_level, cmsg->cmsg_type);
        }

        r = bus_socket_read_message_need(bus, &need);
        if (r < 0)
                return r;

        if (bus->rbuffer_size >= need)
                return bus_socket_make_message(bus, need);

        return 1;
}