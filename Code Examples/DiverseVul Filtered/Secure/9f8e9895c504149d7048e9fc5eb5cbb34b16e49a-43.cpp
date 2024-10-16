int usb_device_delete_addr(int busnr, int addr)
{
    USBBus *bus;
    USBPort *port;
    USBDevice *dev;

    bus = usb_bus_find(busnr);
    if (!bus)
        return -1;

    QTAILQ_FOREACH(port, &bus->used, next) {
        if (port->dev->addr == addr)
            break;
    }
    if (!port)
        return -1;
    dev = port->dev;

    object_unparent(OBJECT(dev));
    return 0;
}