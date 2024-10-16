static void virtio_net_handle_rx(VirtIODevice *vdev, VirtQueue *vq)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    int queue_index = vq2q(virtio_get_queue_index(vq));

    qemu_flush_queued_packets(qemu_get_subqueue(n->nic, queue_index));
}