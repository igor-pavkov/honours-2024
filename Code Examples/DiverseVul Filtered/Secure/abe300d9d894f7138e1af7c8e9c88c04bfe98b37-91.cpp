static int virtio_net_process_rss(NetClientState *nc, const uint8_t *buf,
                                  size_t size)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);
    unsigned int index = nc->queue_index, new_index = index;
    struct NetRxPkt *pkt = n->rx_pkt;
    uint8_t net_hash_type;
    uint32_t hash;
    bool isip4, isip6, isudp, istcp;
    static const uint8_t reports[NetPktRssIpV6UdpEx + 1] = {
        VIRTIO_NET_HASH_REPORT_IPv4,
        VIRTIO_NET_HASH_REPORT_TCPv4,
        VIRTIO_NET_HASH_REPORT_TCPv6,
        VIRTIO_NET_HASH_REPORT_IPv6,
        VIRTIO_NET_HASH_REPORT_IPv6_EX,
        VIRTIO_NET_HASH_REPORT_TCPv6_EX,
        VIRTIO_NET_HASH_REPORT_UDPv4,
        VIRTIO_NET_HASH_REPORT_UDPv6,
        VIRTIO_NET_HASH_REPORT_UDPv6_EX
    };

    net_rx_pkt_set_protocols(pkt, buf + n->host_hdr_len,
                             size - n->host_hdr_len);
    net_rx_pkt_get_protocols(pkt, &isip4, &isip6, &isudp, &istcp);
    if (isip4 && (net_rx_pkt_get_ip4_info(pkt)->fragment)) {
        istcp = isudp = false;
    }
    if (isip6 && (net_rx_pkt_get_ip6_info(pkt)->fragment)) {
        istcp = isudp = false;
    }
    net_hash_type = virtio_net_get_hash_type(isip4, isip6, isudp, istcp,
                                             n->rss_data.hash_types);
    if (net_hash_type > NetPktRssIpV6UdpEx) {
        if (n->rss_data.populate_hash) {
            virtio_set_packet_hash(buf, VIRTIO_NET_HASH_REPORT_NONE, 0);
        }
        return n->rss_data.redirect ? n->rss_data.default_queue : -1;
    }

    hash = net_rx_pkt_calc_rss_hash(pkt, net_hash_type, n->rss_data.key);

    if (n->rss_data.populate_hash) {
        virtio_set_packet_hash(buf, reports[net_hash_type], hash);
    }

    if (n->rss_data.redirect) {
        new_index = hash & (n->rss_data.indirections_len - 1);
        new_index = n->rss_data.indirections_table[new_index];
    }

    return (index == new_index) ? -1 : new_index;
}