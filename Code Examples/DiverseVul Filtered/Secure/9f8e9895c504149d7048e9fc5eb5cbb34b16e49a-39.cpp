int usb_device_alloc_streams(USBDevice *dev, USBEndpoint **eps, int nr_eps,
                             int streams)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->alloc_streams) {
        return klass->alloc_streams(dev, eps, nr_eps, streams);
    }
    return 0;
}