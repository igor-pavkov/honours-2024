static bool net_tx_pkt_parse_headers(struct NetTxPkt *pkt)
{
    struct iovec *l2_hdr, *l3_hdr;
    size_t bytes_read;
    size_t full_ip6hdr_len;
    uint16_t l3_proto;

    assert(pkt);

    l2_hdr = &pkt->vec[NET_TX_PKT_L2HDR_FRAG];
    l3_hdr = &pkt->vec[NET_TX_PKT_L3HDR_FRAG];

    bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, 0, l2_hdr->iov_base,
                            ETH_MAX_L2_HDR_LEN);
    if (bytes_read < sizeof(struct eth_header)) {
        l2_hdr->iov_len = 0;
        return false;
    }

    l2_hdr->iov_len = sizeof(struct eth_header);
    switch (be16_to_cpu(PKT_GET_ETH_HDR(l2_hdr->iov_base)->h_proto)) {
    case ETH_P_VLAN:
        l2_hdr->iov_len += sizeof(struct vlan_header);
        break;
    case ETH_P_DVLAN:
        l2_hdr->iov_len += 2 * sizeof(struct vlan_header);
        break;
    }

    if (bytes_read < l2_hdr->iov_len) {
        l2_hdr->iov_len = 0;
        l3_hdr->iov_len = 0;
        pkt->packet_type = ETH_PKT_UCAST;
        return false;
    } else {
        l2_hdr->iov_len = ETH_MAX_L2_HDR_LEN;
        l2_hdr->iov_len = eth_get_l2_hdr_length(l2_hdr->iov_base);
        pkt->packet_type = get_eth_packet_type(l2_hdr->iov_base);
    }

    l3_proto = eth_get_l3_proto(l2_hdr, 1, l2_hdr->iov_len);

    switch (l3_proto) {
    case ETH_P_IP:
        bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                                l3_hdr->iov_base, sizeof(struct ip_header));

        if (bytes_read < sizeof(struct ip_header)) {
            l3_hdr->iov_len = 0;
            return false;
        }

        l3_hdr->iov_len = IP_HDR_GET_LEN(l3_hdr->iov_base);

        if (l3_hdr->iov_len < sizeof(struct ip_header)) {
            l3_hdr->iov_len = 0;
            return false;
        }

        pkt->l4proto = IP_HDR_GET_P(l3_hdr->iov_base);

        if (IP_HDR_GET_LEN(l3_hdr->iov_base) != sizeof(struct ip_header)) {
            /* copy optional IPv4 header data if any*/
            bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags,
                                    l2_hdr->iov_len + sizeof(struct ip_header),
                                    l3_hdr->iov_base + sizeof(struct ip_header),
                                    l3_hdr->iov_len - sizeof(struct ip_header));
            if (bytes_read < l3_hdr->iov_len - sizeof(struct ip_header)) {
                l3_hdr->iov_len = 0;
                return false;
            }
        }

        break;

    case ETH_P_IPV6:
    {
        eth_ip6_hdr_info hdrinfo;

        if (!eth_parse_ipv6_hdr(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                                &hdrinfo)) {
            l3_hdr->iov_len = 0;
            return false;
        }

        pkt->l4proto = hdrinfo.l4proto;
        full_ip6hdr_len = hdrinfo.full_hdr_len;

        if (full_ip6hdr_len > ETH_MAX_IP_DGRAM_LEN) {
            l3_hdr->iov_len = 0;
            return false;
        }

        bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                                l3_hdr->iov_base, full_ip6hdr_len);

        if (bytes_read < full_ip6hdr_len) {
            l3_hdr->iov_len = 0;
            return false;
        } else {
            l3_hdr->iov_len = full_ip6hdr_len;
        }
        break;
    }
    default:
        l3_hdr->iov_len = 0;
        break;
    }

    net_tx_pkt_calculate_hdr_len(pkt);
    return true;
}