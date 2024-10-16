static void virtio_net_rsc_extract_unit6(VirtioNetRscChain *chain,
                                         const uint8_t *buf,
                                         VirtioNetRscUnit *unit)
{
    struct ip6_header *ip6;

    ip6 = (struct ip6_header *)(buf + chain->n->guest_hdr_len
                                 + sizeof(struct eth_header));
    unit->ip = ip6;
    unit->ip_plen = &(ip6->ip6_ctlun.ip6_un1.ip6_un1_plen);
    unit->tcp = (struct tcp_header *)(((uint8_t *)unit->ip)
                                        + sizeof(struct ip6_header));
    unit->tcp_hdrlen = (htons(unit->tcp->th_offset_flags) & 0xF000) >> 10;

    /* There is a difference between payload lenght in ipv4 and v6,
       ip header is excluded in ipv6 */
    unit->payload = htons(*unit->ip_plen) - unit->tcp_hdrlen;
}