static int virtio_net_max_tx_queue_size(VirtIONet *n)
{
    NetClientState *peer = n->nic_conf.peers.ncs[0];

    /*
     * Backends other than vhost-user or vhost-vdpa don't support max queue
     * size.
     */
    if (!peer) {
        return VIRTIO_NET_TX_QUEUE_DEFAULT_SIZE;
    }

    switch(peer->info->type) {
    case NET_CLIENT_DRIVER_VHOST_USER:
    case NET_CLIENT_DRIVER_VHOST_VDPA:
        return VIRTQUEUE_MAX_SIZE;
    default:
        return VIRTIO_NET_TX_QUEUE_DEFAULT_SIZE;
    };
}