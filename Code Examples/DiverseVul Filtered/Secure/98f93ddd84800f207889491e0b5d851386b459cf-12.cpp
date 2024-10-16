static void virtio_net_device_unrealize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIONet *n = VIRTIO_NET(dev);
    int i;

    /* This will stop vhost backend if appropriate. */
    virtio_net_set_status(vdev, 0);

    unregister_savevm(dev, "virtio-net", n);

    if (n->netclient_name) {
        g_free(n->netclient_name);
        n->netclient_name = NULL;
    }
    if (n->netclient_type) {
        g_free(n->netclient_type);
        n->netclient_type = NULL;
    }

    g_free(n->mac_table.macs);
    g_free(n->vlans);

    for (i = 0; i < n->max_queues; i++) {
        VirtIONetQueue *q = &n->vqs[i];
        NetClientState *nc = qemu_get_subqueue(n->nic, i);

        qemu_purge_queued_packets(nc);

        if (q->tx_timer) {
            timer_del(q->tx_timer);
            timer_free(q->tx_timer);
        } else if (q->tx_bh) {
            qemu_bh_delete(q->tx_bh);
        }
    }

    g_free(n->vqs);
    qemu_del_nic(n->nic);
    virtio_cleanup(vdev);
}