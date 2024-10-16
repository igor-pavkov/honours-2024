int bus_socket_process_watch_bind(sd_bus *b) {
        int r, q;

        assert(b);
        assert(b->state == BUS_WATCH_BIND);
        assert(b->inotify_fd >= 0);

        r = flush_fd(b->inotify_fd);
        if (r <= 0)
                return r;

        log_debug("Got inotify event on bus %s.", strna(b->description));

        /* We flushed events out of the inotify fd. In that case, maybe the socket is valid now? Let's try to connect
         * to it again */

        r = bus_socket_connect(b);
        if (r < 0)
                return r;

        q = bus_attach_io_events(b);
        if (q < 0)
                return q;

        q = bus_attach_inotify_event(b);
        if (q < 0)
                return q;

        return r;
}