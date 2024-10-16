void usb_bus_new(USBBus *bus, size_t bus_size,
                 USBBusOps *ops, DeviceState *host)
{
    qbus_create_inplace(bus, bus_size, TYPE_USB_BUS, host, NULL);
    bus->ops = ops;
    bus->busnr = next_usb_bus++;
    bus->qbus.allow_hotplug = 1; /* Yes, we can */
    QTAILQ_INIT(&bus->free);
    QTAILQ_INIT(&bus->used);
    QTAILQ_INSERT_TAIL(&busses, bus, next);
}