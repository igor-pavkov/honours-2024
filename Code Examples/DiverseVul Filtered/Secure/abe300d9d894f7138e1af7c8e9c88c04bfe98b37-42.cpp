static void virtio_net_rsc_extract_unit4(VirtioNetRscChain *chain,
                                         const uint8_t *buf,
                                         VirtioNetRscUnit *unit)
{
    uint16_t ip_hdrlen;
    struct ip_header *ip;

    ip = (struct ip_header *)(buf + chain->n->guest_hdr_len
                              + sizeof(struct eth_header));
    unit->ip = (void *)ip;
    ip_hdrlen = (ip->ip_ver_len & 0xF) << 2;
    unit->ip_plen = &ip->ip_len;
    unit->tcp = (struct tcp_header *)(((uint8_t *)unit->ip) + ip_hdrlen);
    unit->tcp_hdrlen = (htons(unit->tcp->th_offset_flags) & 0xF000) >> 10;
    unit->payload = htons(*unit->ip_plen) - ip_hdrlen - unit->tcp_hdrlen;
}