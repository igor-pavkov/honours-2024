int bus_socket_write_message(sd_bus *bus, sd_bus_message *m, size_t *idx) {
        struct iovec *iov;
        ssize_t k;
        size_t n;
        unsigned j;
        int r;

        assert(bus);
        assert(m);
        assert(idx);
        assert(IN_SET(bus->state, BUS_RUNNING, BUS_HELLO));

        if (*idx >= BUS_MESSAGE_SIZE(m))
                return 0;

        r = bus_message_setup_iovec(m);
        if (r < 0)
                return r;

        n = m->n_iovec * sizeof(struct iovec);
        iov = newa(struct iovec, n);
        memcpy_safe(iov, m->iovec, n);

        j = 0;
        iovec_advance(iov, &j, *idx);

        if (bus->prefer_writev)
                k = writev(bus->output_fd, iov, m->n_iovec);
        else {
                struct msghdr mh = {
                        .msg_iov = iov,
                        .msg_iovlen = m->n_iovec,
                };

                if (m->n_fds > 0 && *idx == 0) {
                        struct cmsghdr *control;

                        mh.msg_control = control = alloca(CMSG_SPACE(sizeof(int) * m->n_fds));
                        mh.msg_controllen = control->cmsg_len = CMSG_LEN(sizeof(int) * m->n_fds);
                        control->cmsg_level = SOL_SOCKET;
                        control->cmsg_type = SCM_RIGHTS;
                        memcpy(CMSG_DATA(control), m->fds, sizeof(int) * m->n_fds);
                }

                k = sendmsg(bus->output_fd, &mh, MSG_DONTWAIT|MSG_NOSIGNAL);
                if (k < 0 && errno == ENOTSOCK) {
                        bus->prefer_writev = true;
                        k = writev(bus->output_fd, iov, m->n_iovec);
                }
        }

        if (k < 0)
                return errno == EAGAIN ? 0 : -errno;

        *idx += (size_t) k;
        return 1;
}