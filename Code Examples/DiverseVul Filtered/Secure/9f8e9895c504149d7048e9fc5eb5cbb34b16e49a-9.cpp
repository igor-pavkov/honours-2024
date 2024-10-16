USBDevice *usb_create_simple(USBBus *bus, const char *name)
{
    USBDevice *dev = usb_create(bus, name);
    int rc;

    if (!dev) {
        error_report("Failed to create USB device '%s'", name);
        return NULL;
    }
    rc = qdev_init(&dev->qdev);
    if (rc < 0) {
        error_report("Failed to initialize USB device '%s'", name);
        return NULL;
    }
    return dev;
}