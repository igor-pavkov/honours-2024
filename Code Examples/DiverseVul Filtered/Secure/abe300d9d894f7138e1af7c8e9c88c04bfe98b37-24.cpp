static void virtio_net_drop_tx_queue_data(VirtIODevice *vdev, VirtQueue *vq)
{
    unsigned int dropped = virtqueue_drop_all(vq);
    if (dropped) {
        virtio_notify(vdev, vq);
    }
}