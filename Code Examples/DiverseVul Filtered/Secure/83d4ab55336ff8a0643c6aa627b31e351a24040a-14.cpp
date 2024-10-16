static int get_seat_from_display(const char *display, const char **seat, uint32_t *vtnr) {
        union sockaddr_union sa = {};
        _cleanup_free_ char *p = NULL, *tty = NULL;
        _cleanup_close_ int fd = -1;
        struct ucred ucred;
        int v, r, salen;

        assert(display);
        assert(vtnr);

        /* We deduce the X11 socket from the display name, then use
         * SO_PEERCRED to determine the X11 server process, ask for
         * the controlling tty of that and if it's a VC then we know
         * the seat and the virtual terminal. Sounds ugly, is only
         * semi-ugly. */

        r = socket_from_display(display, &p);
        if (r < 0)
                return r;
        salen = sockaddr_un_set_path(&sa.un, p);
        if (salen < 0)
                return salen;

        fd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        if (fd < 0)
                return -errno;

        if (connect(fd, &sa.sa, salen) < 0)
                return -errno;

        r = getpeercred(fd, &ucred);
        if (r < 0)
                return r;

        r = get_ctty(ucred.pid, NULL, &tty);
        if (r < 0)
                return r;

        v = vtnr_from_tty(tty);
        if (v < 0)
                return v;
        else if (v == 0)
                return -ENOENT;

        if (seat)
                *seat = "seat0";
        *vtnr = (uint32_t) v;

        return 0;
}