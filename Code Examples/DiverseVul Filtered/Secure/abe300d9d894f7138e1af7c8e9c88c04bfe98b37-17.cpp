static bool virtio_net_rss_needed(void *opaque)
{
    return VIRTIO_NET(opaque)->rss_data.enabled;
}