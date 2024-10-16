static void virtio_net_announce_timer(void *opaque)
{
    VirtIONet *n = opaque;
    trace_virtio_net_announce_timer(n->announce_timer.round);

    n->announce_timer.round--;
    virtio_net_announce_notify(n);
}