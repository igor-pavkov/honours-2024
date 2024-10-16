static char *usb_get_dev_path(DeviceState *qdev)
{
    USBDevice *dev = USB_DEVICE(qdev);
    DeviceState *hcd = qdev->parent_bus->parent;
    char *id = NULL;

    if (dev->flags & (1 << USB_DEV_FLAG_FULL_PATH)) {
        id = qdev_get_dev_path(hcd);
    }
    if (id) {
        char *ret = g_strdup_printf("%s/%s", id, dev->port->path);
        g_free(id);
        return ret;
    } else {
        return g_strdup(dev->port->path);
    }
}