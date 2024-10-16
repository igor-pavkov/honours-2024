void usb_device_ep_stopped(USBDevice *dev, USBEndpoint *ep)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->ep_stopped) {
        klass->ep_stopped(dev, ep);
    }
}