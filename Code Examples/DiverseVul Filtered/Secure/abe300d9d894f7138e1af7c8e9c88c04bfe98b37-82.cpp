static void virtio_net_del_queue(VirtIONet *n, int index)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    VirtIONetQueue *q = &n->vqs[index];
    NetClientState *nc = qemu_get_subqueue(n->nic, index);

    qemu_purge_queued_packets(nc);

    virtio_del_queue(vdev, index * 2);
    if (q->tx_timer) {
        timer_free(q->tx_timer);
        q->tx_timer = NULL;
    } else {
        qemu_bh_delete(q->tx_bh);
        q->tx_bh = NULL;
    }
    q->tx_waiting = 0;
    virtio_del_queue(vdev, index * 2 + 1);
}