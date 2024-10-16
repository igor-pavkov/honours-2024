static int bus_socket_auth_write(sd_bus *b, const char *t) {
        char *p;
        size_t l;

        assert(b);
        assert(t);

        /* We only make use of the first iovec */
        assert(IN_SET(b->auth_index, 0, 1));

        l = strlen(t);
        p = malloc(b->auth_iovec[0].iov_len + l);
        if (!p)
                return -ENOMEM;

        memcpy_safe(p, b->auth_iovec[0].iov_base, b->auth_iovec[0].iov_len);
        memcpy(p + b->auth_iovec[0].iov_len, t, l);

        b->auth_iovec[0].iov_base = p;
        b->auth_iovec[0].iov_len += l;

        free(b->auth_buffer);
        b->auth_buffer = p;
        b->auth_index = 0;
        return 0;
}