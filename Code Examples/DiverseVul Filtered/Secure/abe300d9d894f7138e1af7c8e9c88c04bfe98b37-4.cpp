static void virtio_net_handle_tx_timer(VirtIODevice *vdev, VirtQueue *vq)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    VirtIONetQueue *q = &n->vqs[vq2q(virtio_get_queue_index(vq))];

    if (unlikely((n->status & VIRTIO_NET_S_LINK_UP) == 0)) {
        virtio_net_drop_tx_queue_data(vdev, vq);
        return;
    }

    /* This happens when device was stopped but VCPU wasn't. */
    if (!vdev->vm_running) {
        q->tx_waiting = 1;
        return;
    }

    if (q->tx_waiting) {
        virtio_queue_set_notification(vq, 1);
        timer_del(q->tx_timer);
        q->tx_waiting = 0;
        if (virtio_net_flush_tx(q) == -EINVAL) {
            return;
        }
    } else {
        timer_mod(q->tx_timer,
                       qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + n->tx_timeout);
        q->tx_waiting = 1;
        virtio_queue_set_notification(vq, 0);
    }
}