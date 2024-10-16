static void virtio_net_set_multiqueue(VirtIONet *n, int multiqueue)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    int i, max = multiqueue ? n->max_queues : 1;

    n->multiqueue = multiqueue;

    for (i = 2; i <= n->max_queues * 2 + 1; i++) {
        virtio_del_queue(vdev, i);
    }

    for (i = 1; i < max; i++) {
        n->vqs[i].rx_vq = virtio_add_queue(vdev, 256, virtio_net_handle_rx);
        if (n->vqs[i].tx_timer) {
            n->vqs[i].tx_vq =
                virtio_add_queue(vdev, 256, virtio_net_handle_tx_timer);
            n->vqs[i].tx_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL,
                                                   virtio_net_tx_timer,
                                                   &n->vqs[i]);
        } else {
            n->vqs[i].tx_vq =
                virtio_add_queue(vdev, 256, virtio_net_handle_tx_bh);
            n->vqs[i].tx_bh = qemu_bh_new(virtio_net_tx_bh, &n->vqs[i]);
        }

        n->vqs[i].tx_waiting = 0;
        n->vqs[i].n = n;
    }

    /* Note: Minux Guests (version 3.2.1) use ctrl vq but don't ack
     * VIRTIO_NET_F_CTRL_VQ. Create ctrl vq unconditionally to avoid
     * breaking them.
     */
    n->ctrl_vq = virtio_add_queue(vdev, 64, virtio_net_handle_ctrl);

    virtio_net_set_queues(n);
}