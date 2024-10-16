void tcp_template(struct tcpcb *tp)
{
    struct socket *so = tp->t_socket;
    register struct tcpiphdr *n = &tp->t_template;

    n->ti_mbuf = NULL;
    memset(&n->ti, 0, sizeof(n->ti));
    n->ti_x0 = 0;
    switch (so->so_ffamily) {
    case AF_INET:
        n->ti_pr = IPPROTO_TCP;
        n->ti_len = htons(sizeof(struct tcphdr));
        n->ti_src = so->so_faddr;
        n->ti_dst = so->so_laddr;
        n->ti_sport = so->so_fport;
        n->ti_dport = so->so_lport;
        break;

    case AF_INET6:
        n->ti_nh6 = IPPROTO_TCP;
        n->ti_len = htons(sizeof(struct tcphdr));
        n->ti_src6 = so->so_faddr6;
        n->ti_dst6 = so->so_laddr6;
        n->ti_sport = so->so_fport6;
        n->ti_dport = so->so_lport6;
        break;

    default:
        g_assert_not_reached();
    }

    n->ti_seq = 0;
    n->ti_ack = 0;
    n->ti_x2 = 0;
    n->ti_off = 5;
    n->ti_flags = 0;
    n->ti_win = 0;
    n->ti_sum = 0;
    n->ti_urp = 0;
}