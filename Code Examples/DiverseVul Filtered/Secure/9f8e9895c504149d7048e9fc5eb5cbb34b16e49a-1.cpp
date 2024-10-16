void usb_device_free_streams(USBDevice *dev, USBEndpoint **eps, int nr_eps)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->free_streams) {
        klass->free_streams(dev, eps, nr_eps);
    }
}