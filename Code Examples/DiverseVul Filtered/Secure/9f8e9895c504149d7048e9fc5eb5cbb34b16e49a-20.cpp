void usb_register_port(USBBus *bus, USBPort *port, void *opaque, int index,
                       USBPortOps *ops, int speedmask)
{
    usb_fill_port(port, opaque, index, ops, speedmask);
    QTAILQ_INSERT_TAIL(&bus->free, port, next);
    bus->nfree++;
}