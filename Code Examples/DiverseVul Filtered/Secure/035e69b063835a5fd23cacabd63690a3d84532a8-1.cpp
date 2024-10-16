void net_tx_pkt_fix_ip6_payload_len(struct NetTxPkt *pkt)
{
    struct iovec *l2 = &pkt->vec[NET_TX_PKT_L2HDR_FRAG];
    if (eth_get_l3_proto(l2, 1, l2->iov_len) == ETH_P_IPV6) {
        struct ip6_header *ip6 = (struct ip6_header *) pkt->l3_hdr;
        /*
         * TODO: if qemu would support >64K packets - add jumbo option check
         * something like that:
         * 'if (ip6->ip6_plen == 0 && !has_jumbo_option(ip6)) {'
         */
        if (ip6->ip6_plen == 0) {
            if (pkt->payload_len <= ETH_MAX_IP_DGRAM_LEN) {
                ip6->ip6_plen = htons(pkt->payload_len);
            }
            /*
             * TODO: if qemu would support >64K packets
             * add jumbo option for packets greater then 65,535 bytes
             */
        }
    }
}