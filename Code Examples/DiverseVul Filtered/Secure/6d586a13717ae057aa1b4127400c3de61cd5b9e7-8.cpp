static int bus_socket_write_auth(sd_bus *b) {
        ssize_t k;

        assert(b);
        assert(b->state == BUS_AUTHENTICATING);

        if (!bus_socket_auth_needs_write(b))
                return 0;

        if (b->prefer_writev)
                k = writev(b->output_fd, b->auth_iovec + b->auth_index, ELEMENTSOF(b->auth_iovec) - b->auth_index);
        else {
                struct msghdr mh;
                zero(mh);

                mh.msg_iov = b->auth_iovec + b->auth_index;
                mh.msg_iovlen = ELEMENTSOF(b->auth_iovec) - b->auth_index;

                k = sendmsg(b->output_fd, &mh, MSG_DONTWAIT|MSG_NOSIGNAL);
                if (k < 0 && errno == ENOTSOCK) {
                        b->prefer_writev = true;
                        k = writev(b->output_fd, b->auth_iovec + b->auth_index, ELEMENTSOF(b->auth_iovec) - b->auth_index);
                }
        }

        if (k < 0)
                return errno == EAGAIN ? 0 : -errno;

        iovec_advance(b->auth_iovec, &b->auth_index, (size_t) k);
        return 1;
}