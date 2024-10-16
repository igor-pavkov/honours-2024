void usbnet_device_suggests_idle(struct usbnet *dev)
{
	if (!test_and_set_bit(EVENT_DEVICE_REPORT_IDLE, &dev->flags)) {
		dev->intf->needs_remote_wakeup = 1;
		usb_autopm_put_interface_async(dev->intf);
	}
}