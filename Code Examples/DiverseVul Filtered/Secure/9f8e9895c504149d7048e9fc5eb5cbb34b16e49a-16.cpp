void usb_device_cancel_packet(USBDevice *dev, USBPacket *p)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->cancel_packet) {
        klass->cancel_packet(dev, p);
    }
}