void ip_stripoptions(register struct mbuf *m, struct mbuf *mopt)
{
    register int i;
    struct ip *ip = mtod(m, struct ip *);
    register char *opts;
    int olen;

    olen = (ip->ip_hl << 2) - sizeof(struct ip);
    opts = (char *)(ip + 1);
    i = m->m_len - (sizeof(struct ip) + olen);
    memcpy(opts, opts + olen, (unsigned)i);
    m->m_len -= olen;

    ip->ip_hl = sizeof(struct ip) >> 2;
}