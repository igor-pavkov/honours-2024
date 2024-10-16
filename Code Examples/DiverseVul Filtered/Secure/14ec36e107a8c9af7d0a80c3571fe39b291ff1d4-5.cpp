static struct tftp_t *tftp_prep_mbuf_data(struct tftp_session *spt,
                                          struct mbuf *m)
{
    struct tftp_t *tp;

    memset(m->m_data, 0, m->m_size);

    m->m_data += IF_MAXLINKHDR;
    if (spt->client_addr.ss_family == AF_INET6) {
        m->m_data += sizeof(struct ip6);
    } else {
        m->m_data += sizeof(struct ip);
    }
    tp = (void *)m->m_data;
    m->m_data += sizeof(struct udphdr);

    return tp;
}