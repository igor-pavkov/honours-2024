void usb_device_flush_ep_queue(USBDevice *dev, USBEndpoint *ep)
{
    USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
    if (klass->flush_ep_queue) {
        klass->flush_ep_queue(dev, ep);
    }
}