static VirtioNetRscChain *virtio_net_rsc_lookup_chain(VirtIONet *n,
                                                      NetClientState *nc,
                                                      uint16_t proto)
{
    VirtioNetRscChain *chain;

    if ((proto != (uint16_t)ETH_P_IP) && (proto != (uint16_t)ETH_P_IPV6)) {
        return NULL;
    }

    QTAILQ_FOREACH(chain, &n->rsc_chains, next) {
        if (chain->proto == proto) {
            return chain;
        }
    }

    chain = g_malloc(sizeof(*chain));
    chain->n = n;
    chain->proto = proto;
    if (proto == (uint16_t)ETH_P_IP) {
        chain->max_payload = VIRTIO_NET_MAX_IP4_PAYLOAD;
        chain->gso_type = VIRTIO_NET_HDR_GSO_TCPV4;
    } else {
        chain->max_payload = VIRTIO_NET_MAX_IP6_PAYLOAD;
        chain->gso_type = VIRTIO_NET_HDR_GSO_TCPV6;
    }
    chain->drain_timer = timer_new_ns(QEMU_CLOCK_HOST,
                                      virtio_net_rsc_purge, chain);
    memset(&chain->stat, 0, sizeof(chain->stat));

    QTAILQ_INIT(&chain->buffers);
    QTAILQ_INSERT_TAIL(&n->rsc_chains, chain, next);

    return chain;
}