USBDevice *usb_device_find_device(USBDevice *dev, uint8_t addr)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->find_device) {
        return klass->find_device(dev, addr);
    }
    return NULL;
}