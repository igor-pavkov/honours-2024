static void virtio_device_unrealize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_GET_CLASS(dev);
    Error *err = NULL;

    virtio_bus_device_unplugged(vdev);

    if (vdc->unrealize != NULL) {
        vdc->unrealize(dev, &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
    }

    if (vdev->bus_name) {
        g_free(vdev->bus_name);
        vdev->bus_name = NULL;
    }
}