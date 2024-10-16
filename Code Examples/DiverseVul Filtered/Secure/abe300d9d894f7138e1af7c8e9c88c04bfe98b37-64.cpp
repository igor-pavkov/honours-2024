static size_t virtio_net_rsc_do_coalesce(VirtioNetRscChain *chain,
                                         NetClientState *nc,
                                         const uint8_t *buf, size_t size,
                                         VirtioNetRscUnit *unit)
{
    int ret;
    VirtioNetRscSeg *seg, *nseg;

    if (QTAILQ_EMPTY(&chain->buffers)) {
        chain->stat.empty_cache++;
        virtio_net_rsc_cache_buf(chain, nc, buf, size);
        timer_mod(chain->drain_timer,
              qemu_clock_get_ns(QEMU_CLOCK_HOST) + chain->n->rsc_timeout);
        return size;
    }

    QTAILQ_FOREACH_SAFE(seg, &chain->buffers, next, nseg) {
        if (chain->proto == ETH_P_IP) {
            ret = virtio_net_rsc_coalesce4(chain, seg, buf, size, unit);
        } else {
            ret = virtio_net_rsc_coalesce6(chain, seg, buf, size, unit);
        }

        if (ret == RSC_FINAL) {
            if (virtio_net_rsc_drain_seg(chain, seg) == 0) {
                /* Send failed */
                chain->stat.final_failed++;
                return 0;
            }

            /* Send current packet */
            return virtio_net_do_receive(nc, buf, size);
        } else if (ret == RSC_NO_MATCH) {
            continue;
        } else {
            /* Coalesced, mark coalesced flag to tell calc cksum for ipv4 */
            seg->is_coalesced = 1;
            return size;
        }
    }

    chain->stat.no_match_cache++;
    virtio_net_rsc_cache_buf(chain, nc, buf, size);
    return size;
}