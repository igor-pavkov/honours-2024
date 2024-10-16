static void virtio_net_get_config(VirtIODevice *vdev, uint8_t *config)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    struct virtio_net_config netcfg;

    stw_p(&netcfg.status, n->status);
    stw_p(&netcfg.max_virtqueue_pairs, n->max_queues);
    memcpy(netcfg.mac, n->mac, ETH_ALEN);
    memcpy(config, &netcfg, n->config_size);
}