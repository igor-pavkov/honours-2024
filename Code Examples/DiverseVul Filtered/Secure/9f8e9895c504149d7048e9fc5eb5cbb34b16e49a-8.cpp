USBDevice *usb_create(USBBus *bus, const char *name)
{
    DeviceState *dev;

    dev = qdev_create(&bus->qbus, name);
    return USB_DEVICE(dev);
}