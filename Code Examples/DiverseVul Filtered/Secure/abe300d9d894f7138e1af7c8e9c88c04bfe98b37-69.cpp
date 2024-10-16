static int32_t virtio_net_rsc_sanity_check6(VirtioNetRscChain *chain,
                                            struct ip6_header *ip6,
                                            const uint8_t *buf, size_t size)
{
    uint16_t ip_len;

    if (((ip6->ip6_ctlun.ip6_un1.ip6_un1_flow & 0xF0) >> 4)
        != IP_HEADER_VERSION_6) {
        return RSC_BYPASS;
    }

    /* Both option and protocol is checked in this */
    if (ip6->ip6_ctlun.ip6_un1.ip6_un1_nxt != IPPROTO_TCP) {
        chain->stat.bypass_not_tcp++;
        return RSC_BYPASS;
    }

    ip_len = htons(ip6->ip6_ctlun.ip6_un1.ip6_un1_plen);
    if (ip_len < sizeof(struct tcp_header) ||
        ip_len > (size - chain->n->guest_hdr_len - sizeof(struct eth_header)
                  - sizeof(struct ip6_header))) {
        chain->stat.ip_hacked++;
        return RSC_BYPASS;
    }

    /* Don't handle packets with ecn flag */
    if (IP6_ECN(ip6->ip6_ctlun.ip6_un3.ip6_un3_ecn)) {
        chain->stat.ip_ecn++;
        return RSC_BYPASS;
    }

    return RSC_CANDIDATE;
}