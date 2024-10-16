static void floppy_bus_create(FDCtrl *fdc, FloppyBus *bus, DeviceState *dev)
{
    qbus_init(bus, sizeof(FloppyBus), TYPE_FLOPPY_BUS, dev, NULL);
    bus->fdc = fdc;
}