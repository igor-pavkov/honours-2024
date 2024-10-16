void usb_device_set_interface(USBDevice *dev, int interface,
                              int alt_old, int alt_new)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->set_interface) {
        klass->set_interface(dev, interface, alt_old, alt_new);
    }
}