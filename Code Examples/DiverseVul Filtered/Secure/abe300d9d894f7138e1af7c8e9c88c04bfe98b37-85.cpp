static uint8_t virtio_net_get_hash_type(bool isip4,
                                        bool isip6,
                                        bool isudp,
                                        bool istcp,
                                        uint32_t types)
{
    if (isip4) {
        if (istcp && (types & VIRTIO_NET_RSS_HASH_TYPE_TCPv4)) {
            return NetPktRssIpV4Tcp;
        }
        if (isudp && (types & VIRTIO_NET_RSS_HASH_TYPE_UDPv4)) {
            return NetPktRssIpV4Udp;
        }
        if (types & VIRTIO_NET_RSS_HASH_TYPE_IPv4) {
            return NetPktRssIpV4;
        }
    } else if (isip6) {
        uint32_t mask = VIRTIO_NET_RSS_HASH_TYPE_TCP_EX |
                        VIRTIO_NET_RSS_HASH_TYPE_TCPv6;

        if (istcp && (types & mask)) {
            return (types & VIRTIO_NET_RSS_HASH_TYPE_TCP_EX) ?
                NetPktRssIpV6TcpEx : NetPktRssIpV6Tcp;
        }
        mask = VIRTIO_NET_RSS_HASH_TYPE_UDP_EX | VIRTIO_NET_RSS_HASH_TYPE_UDPv6;
        if (isudp && (types & mask)) {
            return (types & VIRTIO_NET_RSS_HASH_TYPE_UDP_EX) ?
                NetPktRssIpV6UdpEx : NetPktRssIpV6Udp;
        }
        mask = VIRTIO_NET_RSS_HASH_TYPE_IP_EX | VIRTIO_NET_RSS_HASH_TYPE_IPv6;
        if (types & mask) {
            return (types & VIRTIO_NET_RSS_HASH_TYPE_IP_EX) ?
                NetPktRssIpV6Ex : NetPktRssIpV6;
        }
    }
    return 0xff;
}