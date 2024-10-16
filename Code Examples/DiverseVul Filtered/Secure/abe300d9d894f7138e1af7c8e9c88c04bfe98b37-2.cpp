static void virtio_net_rsc_cache_buf(VirtioNetRscChain *chain,
                                     NetClientState *nc,
                                     const uint8_t *buf, size_t size)
{
    uint16_t hdr_len;
    VirtioNetRscSeg *seg;

    hdr_len = chain->n->guest_hdr_len;
    seg = g_malloc(sizeof(VirtioNetRscSeg));
    seg->buf = g_malloc(hdr_len + sizeof(struct eth_header)
        + sizeof(struct ip6_header) + VIRTIO_NET_MAX_TCP_PAYLOAD);
    memcpy(seg->buf, buf, size);
    seg->size = size;
    seg->packets = 1;
    seg->dup_ack = 0;
    seg->is_coalesced = 0;
    seg->nc = nc;

    QTAILQ_INSERT_TAIL(&chain->buffers, seg, next);
    chain->stat.cache++;

    switch (chain->proto) {
    case ETH_P_IP:
        virtio_net_rsc_extract_unit4(chain, seg->buf, &seg->unit);
        break;
    case ETH_P_IPV6:
        virtio_net_rsc_extract_unit6(chain, seg->buf, &seg->unit);
        break;
    default:
        g_assert_not_reached();
    }
}