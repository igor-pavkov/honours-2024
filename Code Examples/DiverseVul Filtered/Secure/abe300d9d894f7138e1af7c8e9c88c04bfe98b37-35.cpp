static size_t virtio_net_rsc_receive4(VirtioNetRscChain *chain,
                                      NetClientState *nc,
                                      const uint8_t *buf, size_t size)
{
    int32_t ret;
    uint16_t hdr_len;
    VirtioNetRscUnit unit;

    hdr_len = ((VirtIONet *)(chain->n))->guest_hdr_len;

    if (size < (hdr_len + sizeof(struct eth_header) + sizeof(struct ip_header)
        + sizeof(struct tcp_header))) {
        chain->stat.bypass_not_tcp++;
        return virtio_net_do_receive(nc, buf, size);
    }

    virtio_net_rsc_extract_unit4(chain, buf, &unit);
    if (virtio_net_rsc_sanity_check4(chain, unit.ip, buf, size)
        != RSC_CANDIDATE) {
        return virtio_net_do_receive(nc, buf, size);
    }

    ret = virtio_net_rsc_tcp_ctrl_check(chain, unit.tcp);
    if (ret == RSC_BYPASS) {
        return virtio_net_do_receive(nc, buf, size);
    } else if (ret == RSC_FINAL) {
        return virtio_net_rsc_drain_flow(chain, nc, buf, size,
                ((hdr_len + sizeof(struct eth_header)) + 12),
                VIRTIO_NET_IP4_ADDR_SIZE,
                hdr_len + sizeof(struct eth_header) + sizeof(struct ip_header));
    }

    return virtio_net_rsc_do_coalesce(chain, nc, buf, size, &unit);
}