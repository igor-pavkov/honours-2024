void usb_info(Monitor *mon, const QDict *qdict)
{
    USBBus *bus;
    USBDevice *dev;
    USBPort *port;

    if (QTAILQ_EMPTY(&busses)) {
        monitor_printf(mon, "USB support not enabled\n");
        return;
    }

    QTAILQ_FOREACH(bus, &busses, next) {
        QTAILQ_FOREACH(port, &bus->used, next) {
            dev = port->dev;
            if (!dev)
                continue;
            monitor_printf(mon, "  Device %d.%d, Port %s, Speed %s Mb/s, Product %s\n",
                           bus->busnr, dev->addr, port->path, usb_speed(dev->speed),
                           dev->product_desc);
        }
    }
}