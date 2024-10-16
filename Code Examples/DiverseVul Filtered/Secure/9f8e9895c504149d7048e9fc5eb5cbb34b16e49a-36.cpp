int usb_device_detach(USBDevice *dev)
{
    USBBus *bus = usb_bus_from_device(dev);
    USBPort *port = dev->port;

    assert(port != NULL);
    assert(dev->attached);
    trace_usb_port_detach(bus->busnr, port->path);

    usb_detach(port);
    dev->attached--;
    return 0;
}