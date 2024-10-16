const char *usb_device_get_product_desc(USBDevice *dev)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    return klass->product_desc;
}