void usb_device_handle_attach(USBDevice *dev)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->handle_attach) {
        klass->handle_attach(dev);
    }
}