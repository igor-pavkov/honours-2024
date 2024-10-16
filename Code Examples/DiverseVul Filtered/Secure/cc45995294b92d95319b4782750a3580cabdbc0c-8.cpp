void virtio_notify_config(VirtIODevice *vdev)
{
    if (!(vdev->status & VIRTIO_CONFIG_S_DRIVER_OK))
        return;

    vdev->isr |= 0x03;
    virtio_notify_vector(vdev, vdev->config_vector);
}