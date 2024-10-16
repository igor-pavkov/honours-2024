void usb_device_handle_data(USBDevice *dev, USBPacket *p)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->handle_data) {
        klass->handle_data(dev, p);
    }
}