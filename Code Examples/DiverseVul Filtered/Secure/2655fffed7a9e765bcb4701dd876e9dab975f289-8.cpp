void tcp_respond(struct tcpcb *tp, struct tcpiphdr *ti, struct mbuf *m,
                 tcp_seq ack, tcp_seq seq, int flags, unsigned short af)
{
    register int tlen;
    int win = 0;

    DEBUG_CALL("tcp_respond");
    DEBUG_ARG("tp = %p", tp);
    DEBUG_ARG("ti = %p", ti);
    DEBUG_ARG("m = %p", m);
    DEBUG_ARG("ack = %u", ack);
    DEBUG_ARG("seq = %u", seq);
    DEBUG_ARG("flags = %x", flags);

    if (tp)
        win = sbspace(&tp->t_socket->so_rcv);
    if (m == NULL) {
        if (!tp || (m = m_get(tp->t_socket->slirp)) == NULL)
            return;
        tlen = 0;
        m->m_data += IF_MAXLINKHDR;
        *mtod(m, struct tcpiphdr *) = *ti;
        ti = mtod(m, struct tcpiphdr *);
        switch (af) {
        case AF_INET:
            ti->ti.ti_i4.ih_x1 = 0;
            break;
        case AF_INET6:
            ti->ti.ti_i6.ih_x1 = 0;
            break;
        default:
            g_assert_not_reached();
        }
        flags = TH_ACK;
    } else {
        /*
         * ti points into m so the next line is just making
         * the mbuf point to ti
         */
        m->m_data = (char *)ti;

        m->m_len = sizeof(struct tcpiphdr);
        tlen = 0;
#define xchg(a, b, type) \
    {                    \
        type t;          \
        t = a;           \
        a = b;           \
        b = t;           \
    }
        switch (af) {
        case AF_INET:
            xchg(ti->ti_dst.s_addr, ti->ti_src.s_addr, uint32_t);
            xchg(ti->ti_dport, ti->ti_sport, uint16_t);
            break;
        case AF_INET6:
            xchg(ti->ti_dst6, ti->ti_src6, struct in6_addr);
            xchg(ti->ti_dport, ti->ti_sport, uint16_t);
            break;
        default:
            g_assert_not_reached();
        }
#undef xchg
    }
    ti->ti_len = htons((uint16_t)(sizeof(struct tcphdr) + tlen));
    tlen += sizeof(struct tcpiphdr);
    m->m_len = tlen;

    ti->ti_mbuf = NULL;
    ti->ti_x0 = 0;
    ti->ti_seq = htonl(seq);
    ti->ti_ack = htonl(ack);
    ti->ti_x2 = 0;
    ti->ti_off = sizeof(struct tcphdr) >> 2;
    ti->ti_flags = flags;
    if (tp)
        ti->ti_win = htons((uint16_t)(win >> tp->rcv_scale));
    else
        ti->ti_win = htons((uint16_t)win);
    ti->ti_urp = 0;
    ti->ti_sum = 0;
    ti->ti_sum = cksum(m, tlen);

    struct tcpiphdr tcpiph_save = *(mtod(m, struct tcpiphdr *));
    struct ip *ip;
    struct ip6 *ip6;

    switch (af) {
    case AF_INET:
        m->m_data +=
            sizeof(struct tcpiphdr) - sizeof(struct tcphdr) - sizeof(struct ip);
        m->m_len -=
            sizeof(struct tcpiphdr) - sizeof(struct tcphdr) - sizeof(struct ip);
        ip = mtod(m, struct ip *);
        ip->ip_len = m->m_len;
        ip->ip_dst = tcpiph_save.ti_dst;
        ip->ip_src = tcpiph_save.ti_src;
        ip->ip_p = tcpiph_save.ti_pr;

        if (flags & TH_RST) {
            ip->ip_ttl = MAXTTL;
        } else {
            ip->ip_ttl = IPDEFTTL;
        }

        ip_output(NULL, m);
        break;

    case AF_INET6:
        m->m_data += sizeof(struct tcpiphdr) - sizeof(struct tcphdr) -
                     sizeof(struct ip6);
        m->m_len -= sizeof(struct tcpiphdr) - sizeof(struct tcphdr) -
                    sizeof(struct ip6);
        ip6 = mtod(m, struct ip6 *);
        ip6->ip_pl = tcpiph_save.ti_len;
        ip6->ip_dst = tcpiph_save.ti_dst6;
        ip6->ip_src = tcpiph_save.ti_src6;
        ip6->ip_nh = tcpiph_save.ti_nh6;

        ip6_output(NULL, m, 0);
        break;

    default:
        g_assert_not_reached();
    }
}