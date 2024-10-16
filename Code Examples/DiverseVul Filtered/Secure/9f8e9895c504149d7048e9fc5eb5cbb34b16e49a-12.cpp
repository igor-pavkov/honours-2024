void usb_unregister_port(USBBus *bus, USBPort *port)
{
    if (port->dev) {
        object_unparent(OBJECT(port->dev));
    }
    QTAILQ_REMOVE(&bus->free, port, next);
    bus->nfree--;
}