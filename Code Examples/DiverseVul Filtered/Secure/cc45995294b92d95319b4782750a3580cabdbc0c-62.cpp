void virtio_device_set_child_bus_name(VirtIODevice *vdev, char *bus_name)
{
    if (vdev->bus_name) {
        g_free(vdev->bus_name);
        vdev->bus_name = NULL;
    }

    if (bus_name) {
        vdev->bus_name = g_strdup(bus_name);
    }
}