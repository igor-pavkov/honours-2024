bool bus_socket_auth_needs_write(sd_bus *b) {

        unsigned i;

        if (b->auth_index >= ELEMENTSOF(b->auth_iovec))
                return false;

        for (i = b->auth_index; i < ELEMENTSOF(b->auth_iovec); i++) {
                struct iovec *j = b->auth_iovec + i;

                if (j->iov_len > 0)
                        return true;
        }

        return false;
}