void virtio_irq(VirtQueue *vq)
{
    trace_virtio_irq(vq);
    vq->vdev->isr |= 0x01;
    virtio_notify_vector(vq->vdev, vq->vector);
}