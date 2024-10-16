struct tcpcb *tcp_newtcpcb(struct socket *so)
{
    register struct tcpcb *tp;

    tp = g_new0(struct tcpcb, 1);
    tp->seg_next = tp->seg_prev = (struct tcpiphdr *)tp;
    /*
     * 40: length of IPv4 header (20) + TCP header (20)
     * 60: length of IPv6 header (40) + TCP header (20)
     */
    tp->t_maxseg =
        MIN(so->slirp->if_mtu - ((so->so_ffamily == AF_INET) ? 40 : 60),
            TCP_MAXSEG_MAX);

    tp->t_flags = TCP_DO_RFC1323 ? (TF_REQ_SCALE | TF_REQ_TSTMP) : 0;
    tp->t_socket = so;

    /*
     * Init srtt to TCPTV_SRTTBASE (0), so we can tell that we have no
     * rtt estimate.  Set rttvar so that srtt + 2 * rttvar gives
     * reasonable initial retransmit time.
     */
    tp->t_srtt = TCPTV_SRTTBASE;
    tp->t_rttvar = TCPTV_SRTTDFLT << 2;
    tp->t_rttmin = TCPTV_MIN;

    TCPT_RANGESET(tp->t_rxtcur,
                  ((TCPTV_SRTTBASE >> 2) + (TCPTV_SRTTDFLT << 2)) >> 1,
                  TCPTV_MIN, TCPTV_REXMTMAX);

    tp->snd_cwnd = TCP_MAXWIN << TCP_MAX_WINSHIFT;
    tp->snd_ssthresh = TCP_MAXWIN << TCP_MAX_WINSHIFT;
    tp->t_state = TCPS_CLOSED;

    so->so_tcpcb = tp;

    return (tp);
}