static void virtio_net_rsc_purge(void *opq)
{
    VirtioNetRscSeg *seg, *rn;
    VirtioNetRscChain *chain = (VirtioNetRscChain *)opq;

    QTAILQ_FOREACH_SAFE(seg, &chain->buffers, next, rn) {
        if (virtio_net_rsc_drain_seg(chain, seg) == 0) {
            chain->stat.purge_failed++;
            continue;
        }
    }

    chain->stat.timer++;
    if (!QTAILQ_EMPTY(&chain->buffers)) {
        timer_mod(chain->drain_timer,
              qemu_clock_get_ns(QEMU_CLOCK_HOST) + chain->n->rsc_timeout);
    }
}