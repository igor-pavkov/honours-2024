static int bus_socket_read_auth(sd_bus *b) {
        struct msghdr mh;
        struct iovec iov = {};
        size_t n;
        ssize_t k;
        int r;
        void *p;
        union {
                struct cmsghdr cmsghdr;
                uint8_t buf[CMSG_SPACE(sizeof(int) * BUS_FDS_MAX)];
        } control;
        bool handle_cmsg = false;

        assert(b);
        assert(b->state == BUS_AUTHENTICATING);

        r = bus_socket_auth_verify(b);
        if (r != 0)
                return r;

        n = MAX(256u, b->rbuffer_size * 2);

        if (n > BUS_AUTH_SIZE_MAX)
                n = BUS_AUTH_SIZE_MAX;

        if (b->rbuffer_size >= n)
                return -ENOBUFS;

        p = realloc(b->rbuffer, n);
        if (!p)
                return -ENOMEM;

        b->rbuffer = p;

        iov = IOVEC_MAKE((uint8_t *)b->rbuffer + b->rbuffer_size, n - b->rbuffer_size);

        if (b->prefer_readv)
                k = readv(b->input_fd, &iov, 1);
        else {
                zero(mh);
                mh.msg_iov = &iov;
                mh.msg_iovlen = 1;
                mh.msg_control = &control;
                mh.msg_controllen = sizeof(control);

                k = recvmsg(b->input_fd, &mh, MSG_DONTWAIT|MSG_CMSG_CLOEXEC);
                if (k < 0 && errno == ENOTSOCK) {
                        b->prefer_readv = true;
                        k = readv(b->input_fd, &iov, 1);
                } else
                        handle_cmsg = true;
        }
        if (k < 0)
                return errno == EAGAIN ? 0 : -errno;
        if (k == 0)
                return -ECONNRESET;

        b->rbuffer_size += k;

        if (handle_cmsg) {
                struct cmsghdr *cmsg;

                CMSG_FOREACH(cmsg, &mh)
                        if (cmsg->cmsg_level == SOL_SOCKET &&
                            cmsg->cmsg_type == SCM_RIGHTS) {
                                int j;

                                /* Whut? We received fds during the auth
                                 * protocol? Somebody is playing games with
                                 * us. Close them all, and fail */
                                j = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);
                                close_many((int*) CMSG_DATA(cmsg), j);
                                return -EIO;
                        } else
                                log_debug("Got unexpected auxiliary data with level=%d and type=%d",
                                          cmsg->cmsg_level, cmsg->cmsg_type);
        }

        r = bus_socket_auth_verify(b);
        if (r != 0)
                return r;

        return 1;
}