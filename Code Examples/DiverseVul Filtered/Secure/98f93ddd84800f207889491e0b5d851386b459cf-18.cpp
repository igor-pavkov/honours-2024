static int virtio_net_has_buffers(VirtIONetQueue *q, int bufsize)
{
    VirtIONet *n = q->n;
    if (virtio_queue_empty(q->rx_vq) ||
        (n->mergeable_rx_bufs &&
         !virtqueue_avail_bytes(q->rx_vq, bufsize, 0))) {
        virtio_queue_set_notification(q->rx_vq, 1);

        /* To avoid a race condition where the guest has made some buffers
         * available after the above check but before notification was
         * enabled, check for available buffers again.
         */
        if (virtio_queue_empty(q->rx_vq) ||
            (n->mergeable_rx_bufs &&
             !virtqueue_avail_bytes(q->rx_vq, bufsize, 0))) {
            return 0;
        }
    }

    virtio_queue_set_notification(q->rx_vq, 0);
    return 1;
}