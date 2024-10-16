void usb_release_port(USBDevice *dev)
{
    USBBus *bus = usb_bus_from_device(dev);
    USBPort *port = dev->port;

    assert(port != NULL);
    trace_usb_port_release(bus->busnr, port->path);

    QTAILQ_REMOVE(&bus->used, port, next);
    bus->nused--;

    dev->port = NULL;
    port->dev = NULL;

    QTAILQ_INSERT_TAIL(&bus->free, port, next);
    bus->nfree++;
}