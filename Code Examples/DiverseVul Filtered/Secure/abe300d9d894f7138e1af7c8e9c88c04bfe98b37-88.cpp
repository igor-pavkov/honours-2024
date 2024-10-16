static int32_t virtio_net_rsc_sanity_check4(VirtioNetRscChain *chain,
                                            struct ip_header *ip,
                                            const uint8_t *buf, size_t size)
{
    uint16_t ip_len;

    /* Not an ipv4 packet */
    if (((ip->ip_ver_len & 0xF0) >> 4) != IP_HEADER_VERSION_4) {
        chain->stat.ip_option++;
        return RSC_BYPASS;
    }

    /* Don't handle packets with ip option */
    if ((ip->ip_ver_len & 0xF) != VIRTIO_NET_IP4_HEADER_LENGTH) {
        chain->stat.ip_option++;
        return RSC_BYPASS;
    }

    if (ip->ip_p != IPPROTO_TCP) {
        chain->stat.bypass_not_tcp++;
        return RSC_BYPASS;
    }

    /* Don't handle packets with ip fragment */
    if (!(htons(ip->ip_off) & IP_DF)) {
        chain->stat.ip_frag++;
        return RSC_BYPASS;
    }

    /* Don't handle packets with ecn flag */
    if (IPTOS_ECN(ip->ip_tos)) {
        chain->stat.ip_ecn++;
        return RSC_BYPASS;
    }

    ip_len = htons(ip->ip_len);
    if (ip_len < (sizeof(struct ip_header) + sizeof(struct tcp_header))
        || ip_len > (size - chain->n->guest_hdr_len -
                     sizeof(struct eth_header))) {
        chain->stat.ip_hacked++;
        return RSC_BYPASS;
    }

    return RSC_CANDIDATE;
}