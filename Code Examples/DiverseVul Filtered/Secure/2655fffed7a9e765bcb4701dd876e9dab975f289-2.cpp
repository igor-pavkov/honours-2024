struct tcpcb *tcp_close(struct tcpcb *tp)
{
    register struct tcpiphdr *t;
    struct socket *so = tp->t_socket;
    Slirp *slirp = so->slirp;
    register struct mbuf *m;

    DEBUG_CALL("tcp_close");
    DEBUG_ARG("tp = %p", tp);

    /* free the reassembly queue, if any */
    t = tcpfrag_list_first(tp);
    while (!tcpfrag_list_end(t, tp)) {
        t = tcpiphdr_next(t);
        m = tcpiphdr_prev(t)->ti_mbuf;
        remque(tcpiphdr2qlink(tcpiphdr_prev(t)));
        m_free(m);
    }
    g_free(tp);
    so->so_tcpcb = NULL;
    /* clobber input socket cache if we're closing the cached connection */
    if (so == slirp->tcp_last_so)
        slirp->tcp_last_so = &slirp->tcb;
    so->slirp->cb->unregister_poll_fd(so->s, so->slirp->opaque);
    closesocket(so->s);
    sbfree(&so->so_rcv);
    sbfree(&so->so_snd);
    sofree(so);
    return ((struct tcpcb *)0);
}