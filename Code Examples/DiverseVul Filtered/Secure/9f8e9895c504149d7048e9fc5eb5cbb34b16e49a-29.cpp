static void usb_bus_dev_print(Monitor *mon, DeviceState *qdev, int indent)
{
    USBDevice *dev = USB_DEVICE(qdev);
    USBBus *bus = usb_bus_from_device(dev);

    monitor_printf(mon, "%*saddr %d.%d, port %s, speed %s, name %s%s\n",
                   indent, "", bus->busnr, dev->addr,
                   dev->port ? dev->port->path : "-",
                   usb_speed(dev->speed), dev->product_desc,
                   dev->attached ? ", attached" : "");
}