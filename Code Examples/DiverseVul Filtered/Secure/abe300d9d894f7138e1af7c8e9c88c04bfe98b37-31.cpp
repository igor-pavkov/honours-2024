static void virtio_net_rsc_cleanup(VirtIONet *n)
{
    VirtioNetRscChain *chain, *rn_chain;
    VirtioNetRscSeg *seg, *rn_seg;

    QTAILQ_FOREACH_SAFE(chain, &n->rsc_chains, next, rn_chain) {
        QTAILQ_FOREACH_SAFE(seg, &chain->buffers, next, rn_seg) {
            QTAILQ_REMOVE(&chain->buffers, seg, next);
            g_free(seg->buf);
            g_free(seg);
        }

        timer_free(chain->drain_timer);
        QTAILQ_REMOVE(&n->rsc_chains, chain, next);
        g_free(chain);
    }
}