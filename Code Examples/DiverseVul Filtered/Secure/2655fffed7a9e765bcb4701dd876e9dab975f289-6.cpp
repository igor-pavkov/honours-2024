void tcp_connect(struct socket *inso)
{
    Slirp *slirp = inso->slirp;
    struct socket *so;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    struct tcpcb *tp;
    int s, opt;

    DEBUG_CALL("tcp_connect");
    DEBUG_ARG("inso = %p", inso);

    /*
     * If it's an SS_ACCEPTONCE socket, no need to socreate()
     * another socket, just use the accept() socket.
     */
    if (inso->so_state & SS_FACCEPTONCE) {
        /* FACCEPTONCE already have a tcpcb */
        so = inso;
    } else {
        so = socreate(slirp);
        tcp_attach(so);
        so->lhost = inso->lhost;
        so->so_ffamily = inso->so_ffamily;
    }

    tcp_mss(sototcpcb(so), 0);

    s = accept(inso->s, (struct sockaddr *)&addr, &addrlen);
    if (s < 0) {
        tcp_close(sototcpcb(so)); /* This will sofree() as well */
        return;
    }
    slirp_set_nonblock(s);
    so->slirp->cb->register_poll_fd(so->s, so->slirp->opaque);
    slirp_socket_set_fast_reuse(s);
    opt = 1;
    setsockopt(s, SOL_SOCKET, SO_OOBINLINE, &opt, sizeof(int));
    slirp_socket_set_nodelay(s);

    so->fhost.ss = addr;
    sotranslate_accept(so);

    /* Close the accept() socket, set right state */
    if (inso->so_state & SS_FACCEPTONCE) {
        /* If we only accept once, close the accept() socket */
        so->slirp->cb->unregister_poll_fd(so->s, so->slirp->opaque);
        closesocket(so->s);

        /* Don't select it yet, even though we have an FD */
        /* if it's not FACCEPTONCE, it's already NOFDREF */
        so->so_state = SS_NOFDREF;
    }
    so->s = s;
    so->so_state |= SS_INCOMING;

    so->so_iptos = tcp_tos(so);
    tp = sototcpcb(so);

    tcp_template(tp);

    tp->t_state = TCPS_SYN_SENT;
    tp->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
    tp->iss = slirp->tcp_iss;
    slirp->tcp_iss += TCP_ISSINCR / 2;
    tcp_sendseqinit(tp);
    tcp_output(tp);
}