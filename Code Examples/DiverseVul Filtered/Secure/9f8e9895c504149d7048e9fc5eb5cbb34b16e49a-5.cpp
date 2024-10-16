static void usb_device_handle_destroy(USBDevice *dev)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->handle_destroy) {
        klass->handle_destroy(dev);
    }
}