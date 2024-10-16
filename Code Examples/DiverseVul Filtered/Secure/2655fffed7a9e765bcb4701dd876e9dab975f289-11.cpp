int tcp_fconnect(struct socket *so, unsigned short af)
{
    int ret = 0;

    DEBUG_CALL("tcp_fconnect");
    DEBUG_ARG("so = %p", so);

    ret = so->s = slirp_socket(af, SOCK_STREAM, 0);
    if (ret >= 0) {
        int opt, s = so->s;
        struct sockaddr_storage addr;

        slirp_set_nonblock(s);
        so->slirp->cb->register_poll_fd(so->s, so->slirp->opaque);
        slirp_socket_set_fast_reuse(s);
        opt = 1;
        setsockopt(s, SOL_SOCKET, SO_OOBINLINE, &opt, sizeof(opt));
        opt = 1;
        setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

        addr = so->fhost.ss;
        DEBUG_CALL(" connect()ing");
        if (sotranslate_out(so, &addr) < 0) {
            return -1;
        }

        /* We don't care what port we get */
        ret = connect(s, (struct sockaddr *)&addr, sockaddr_size(&addr));

        /*
         * If it's not in progress, it failed, so we just return 0,
         * without clearing SS_NOFDREF
         */
        soisfconnecting(so);
    }

    return (ret);
}