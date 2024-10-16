static void net_tx_pkt_do_sw_csum(struct NetTxPkt *pkt)
{
    struct iovec *iov = &pkt->vec[NET_TX_PKT_L2HDR_FRAG];
    uint32_t csum_cntr;
    uint16_t csum = 0;
    uint32_t cso;
    /* num of iovec without vhdr */
    uint32_t iov_len = pkt->payload_frags + NET_TX_PKT_PL_START_FRAG - 1;
    uint16_t csl;
    size_t csum_offset = pkt->virt_hdr.csum_start + pkt->virt_hdr.csum_offset;
    uint16_t l3_proto = eth_get_l3_proto(iov, 1, iov->iov_len);

    /* Put zero to checksum field */
    iov_from_buf(iov, iov_len, csum_offset, &csum, sizeof csum);

    /* Calculate L4 TCP/UDP checksum */
    csl = pkt->payload_len;

    csum_cntr = 0;
    cso = 0;
    /* add pseudo header to csum */
    if (l3_proto == ETH_P_IP) {
        csum_cntr = eth_calc_ip4_pseudo_hdr_csum(
                pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_base,
                csl, &cso);
    } else if (l3_proto == ETH_P_IPV6) {
        csum_cntr = eth_calc_ip6_pseudo_hdr_csum(
                pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_base,
                csl, pkt->l4proto, &cso);
    }

    /* data checksum */
    csum_cntr +=
        net_checksum_add_iov(iov, iov_len, pkt->virt_hdr.csum_start, csl, cso);

    /* Put the checksum obtained into the packet */
    csum = cpu_to_be16(net_checksum_finish_nozero(csum_cntr));
    iov_from_buf(iov, iov_len, csum_offset, &csum, sizeof csum);
}