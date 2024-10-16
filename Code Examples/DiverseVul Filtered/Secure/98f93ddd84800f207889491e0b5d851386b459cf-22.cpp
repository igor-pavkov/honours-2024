static void virtio_net_set_status(struct VirtIODevice *vdev, uint8_t status)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    VirtIONetQueue *q;
    int i;
    uint8_t queue_status;

    virtio_net_vhost_status(n, status);

    for (i = 0; i < n->max_queues; i++) {
        q = &n->vqs[i];

        if ((!n->multiqueue && i != 0) || i >= n->curr_queues) {
            queue_status = 0;
        } else {
            queue_status = status;
        }

        if (!q->tx_waiting) {
            continue;
        }

        if (virtio_net_started(n, queue_status) && !n->vhost_started) {
            if (q->tx_timer) {
                timer_mod(q->tx_timer,
                               qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + n->tx_timeout);
            } else {
                qemu_bh_schedule(q->tx_bh);
            }
        } else {
            if (q->tx_timer) {
                timer_del(q->tx_timer);
            } else {
                qemu_bh_cancel(q->tx_bh);
            }
        }
    }
}