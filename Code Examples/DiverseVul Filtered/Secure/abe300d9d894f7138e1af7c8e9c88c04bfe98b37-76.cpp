static size_t virtio_net_rsc_receive6(void *opq, NetClientState *nc,
                                      const uint8_t *buf, size_t size)
{
    int32_t ret;
    uint16_t hdr_len;
    VirtioNetRscChain *chain;
    VirtioNetRscUnit unit;

    chain = (VirtioNetRscChain *)opq;
    hdr_len = ((VirtIONet *)(chain->n))->guest_hdr_len;

    if (size < (hdr_len + sizeof(struct eth_header) + sizeof(struct ip6_header)
        + sizeof(tcp_header))) {
        return virtio_net_do_receive(nc, buf, size);
    }

    virtio_net_rsc_extract_unit6(chain, buf, &unit);
    if (RSC_CANDIDATE != virtio_net_rsc_sanity_check6(chain,
                                                 unit.ip, buf, size)) {
        return virtio_net_do_receive(nc, buf, size);
    }

    ret = virtio_net_rsc_tcp_ctrl_check(chain, unit.tcp);
    if (ret == RSC_BYPASS) {
        return virtio_net_do_receive(nc, buf, size);
    } else if (ret == RSC_FINAL) {
        return virtio_net_rsc_drain_flow(chain, nc, buf, size,
                ((hdr_len + sizeof(struct eth_header)) + 8),
                VIRTIO_NET_IP6_ADDR_SIZE,
                hdr_len + sizeof(struct eth_header)
                + sizeof(struct ip6_header));
    }

    return virtio_net_rsc_do_coalesce(chain, nc, buf, size, &unit);
}