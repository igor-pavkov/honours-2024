static int bus_socket_start_auth_client(sd_bus *b) {
        size_t l;
        const char *auth_suffix, *auth_prefix;

        assert(b);

        if (b->anonymous_auth) {
                auth_prefix = "\0AUTH ANONYMOUS ";

                /* For ANONYMOUS auth we send some arbitrary "trace" string */
                l = 9;
                b->auth_buffer = hexmem("anonymous", l);
        } else {
                char text[DECIMAL_STR_MAX(uid_t) + 1];

                auth_prefix = "\0AUTH EXTERNAL ";

                xsprintf(text, UID_FMT, geteuid());

                l = strlen(text);
                b->auth_buffer = hexmem(text, l);
        }

        if (!b->auth_buffer)
                return -ENOMEM;

        if (b->accept_fd)
                auth_suffix = "\r\nNEGOTIATE_UNIX_FD\r\nBEGIN\r\n";
        else
                auth_suffix = "\r\nBEGIN\r\n";

        b->auth_iovec[0] = IOVEC_MAKE((void*) auth_prefix, 1 + strlen(auth_prefix + 1));
        b->auth_iovec[1] = IOVEC_MAKE(b->auth_buffer, l * 2);
        b->auth_iovec[2] = IOVEC_MAKE_STRING(auth_suffix);

        return bus_socket_write_auth(b);
}