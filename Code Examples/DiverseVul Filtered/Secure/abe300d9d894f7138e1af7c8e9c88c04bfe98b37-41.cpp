static size_t virtio_net_rsc_drain_flow(VirtioNetRscChain *chain,
                                        NetClientState *nc,
                                        const uint8_t *buf, size_t size,
                                        uint16_t ip_start, uint16_t ip_size,
                                        uint16_t tcp_port)
{
    VirtioNetRscSeg *seg, *nseg;
    uint32_t ppair1, ppair2;

    ppair1 = *(uint32_t *)(buf + tcp_port);
    QTAILQ_FOREACH_SAFE(seg, &chain->buffers, next, nseg) {
        ppair2 = *(uint32_t *)(seg->buf + tcp_port);
        if (memcmp(buf + ip_start, seg->buf + ip_start, ip_size)
            || (ppair1 != ppair2)) {
            continue;
        }
        if (virtio_net_rsc_drain_seg(chain, seg) == 0) {
            chain->stat.drain_failed++;
        }

        break;
    }

    return virtio_net_do_receive(nc, buf, size);
}