static void virtio_net_disable_rss(VirtIONet *n)
{
    if (n->rss_data.enabled) {
        trace_virtio_net_rss_disable();
    }
    n->rss_data.enabled = false;

    virtio_net_detach_epbf_rss(n);
}