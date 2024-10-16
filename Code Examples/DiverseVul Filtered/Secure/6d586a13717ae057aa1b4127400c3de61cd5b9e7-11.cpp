int bus_socket_connect(sd_bus *b) {
        bool inotify_done = false;
        int r;

        assert(b);

        for (;;) {
                assert(b->input_fd < 0);
                assert(b->output_fd < 0);
                assert(b->sockaddr.sa.sa_family != AF_UNSPEC);

                b->input_fd = socket(b->sockaddr.sa.sa_family, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
                if (b->input_fd < 0)
                        return -errno;

                b->input_fd = fd_move_above_stdio(b->input_fd);

                b->output_fd = b->input_fd;
                bus_socket_setup(b);

                if (connect(b->input_fd, &b->sockaddr.sa, b->sockaddr_size) < 0) {
                        if (errno == EINPROGRESS) {

                                /* If we have any inotify watches open, close them now, we don't need them anymore, as
                                 * we have successfully initiated a connection */
                                bus_close_inotify_fd(b);

                                /* Note that very likely we are already in BUS_OPENING state here, as we enter it when
                                 * we start parsing the address string. The only reason we set the state explicitly
                                 * here, is to undo BUS_WATCH_BIND, in case we did the inotify magic. */
                                bus_set_state(b, BUS_OPENING);
                                return 1;
                        }

                        if (IN_SET(errno, ENOENT, ECONNREFUSED) &&  /* ENOENT → unix socket doesn't exist at all; ECONNREFUSED → unix socket stale */
                            b->watch_bind &&
                            b->sockaddr.sa.sa_family == AF_UNIX &&
                            b->sockaddr.un.sun_path[0] != 0) {

                                /* This connection attempt failed, let's release the socket for now, and start with a
                                 * fresh one when reconnecting. */
                                bus_close_io_fds(b);

                                if (inotify_done) {
                                        /* inotify set up already, don't do it again, just return now, and remember
                                         * that we are waiting for inotify events now. */
                                        bus_set_state(b, BUS_WATCH_BIND);
                                        return 1;
                                }

                                /* This is a file system socket, and the inotify logic is enabled. Let's create the necessary inotify fd. */
                                r = bus_socket_inotify_setup(b);
                                if (r < 0)
                                        return r;

                                /* Let's now try to connect a second time, because in theory there's otherwise a race
                                 * here: the socket might have been created in the time between our first connect() and
                                 * the time we set up the inotify logic. But let's remember that we set up inotify now,
                                 * so that we don't do the connect() more than twice. */
                                inotify_done = true;

                        } else
                                return -errno;
                } else
                        break;
        }

        /* Yay, established, we don't need no inotify anymore! */
        bus_close_inotify_fd(b);

        return bus_socket_start_auth(b);
}