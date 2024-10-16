static ssize_t virtio_net_rsc_receive(NetClientState *nc,
                                      const uint8_t *buf,
                                      size_t size)
{
    uint16_t proto;
    VirtioNetRscChain *chain;
    struct eth_header *eth;
    VirtIONet *n;

    n = qemu_get_nic_opaque(nc);
    if (size < (n->host_hdr_len + sizeof(struct eth_header))) {
        return virtio_net_do_receive(nc, buf, size);
    }

    eth = (struct eth_header *)(buf + n->guest_hdr_len);
    proto = htons(eth->h_proto);

    chain = virtio_net_rsc_lookup_chain(n, nc, proto);
    if (chain) {
        chain->stat.received++;
        if (proto == (uint16_t)ETH_P_IP && n->rsc4_enabled) {
            return virtio_net_rsc_receive4(chain, nc, buf, size);
        } else if (proto == (uint16_t)ETH_P_IPV6 && n->rsc6_enabled) {
            return virtio_net_rsc_receive6(chain, nc, buf, size);
        }
    }
    return virtio_net_do_receive(nc, buf, size);
}