USBBus *usb_bus_find(int busnr)
{
    USBBus *bus;

    if (-1 == busnr)
        return QTAILQ_FIRST(&busses);
    QTAILQ_FOREACH(bus, &busses, next) {
        if (bus->busnr == busnr)
            return bus;
    }
    return NULL;
}