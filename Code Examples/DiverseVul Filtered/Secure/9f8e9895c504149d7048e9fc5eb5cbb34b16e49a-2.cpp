const USBDesc *usb_device_get_usb_desc(USBDevice *dev)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (dev->usb_desc) {
        return dev->usb_desc;
    }
    return klass->usb_desc;
}