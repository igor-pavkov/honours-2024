void virtio_notify(VirtIODevice *vdev, VirtQueue *vq)
{
    if (!vring_notify(vdev, vq)) {
        return;
    }

    trace_virtio_notify(vdev, vq);
    vdev->isr |= 0x01;
    virtio_notify_vector(vdev, vq->vector);
}