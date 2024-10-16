int usb_register_companion(const char *masterbus, USBPort *ports[],
                           uint32_t portcount, uint32_t firstport,
                           void *opaque, USBPortOps *ops, int speedmask)
{
    USBBus *bus;
    int i;

    QTAILQ_FOREACH(bus, &busses, next) {
        if (strcmp(bus->qbus.name, masterbus) == 0) {
            break;
        }
    }

    if (!bus || !bus->ops->register_companion) {
        qerror_report(QERR_INVALID_PARAMETER_VALUE, "masterbus",
                      "an USB masterbus");
        if (bus) {
            error_printf_unless_qmp(
                "USB bus '%s' does not allow companion controllers\n",
                masterbus);
        }
        return -1;
    }

    for (i = 0; i < portcount; i++) {
        usb_fill_port(ports[i], opaque, i, ops, speedmask);
    }

    return bus->ops->register_companion(bus, ports, portcount, firstport);
}