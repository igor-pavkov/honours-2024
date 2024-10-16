void virtio_update_irq(VirtIODevice *vdev)
{
    virtio_notify_vector(vdev, VIRTIO_NO_VECTOR);
}