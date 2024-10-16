void usb_device_handle_control(USBDevice *dev, USBPacket *p, int request,
                               int value, int index, int length, uint8_t *data)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->handle_control) {
        klass->handle_control(dev, p, request, value, index, length, data);
    }
}