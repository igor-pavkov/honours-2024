static void smsusb_term_device(struct usb_interface *intf)
{
	struct smsusb_device_t *dev = usb_get_intfdata(intf);

	if (dev) {
		dev->state = SMSUSB_DISCONNECTED;

		smsusb_stop_streaming(dev);

		/* unregister from smscore */
		if (dev->coredev)
			smscore_unregister_device(dev->coredev);

		pr_debug("device 0x%p destroyed\n", dev);
		kfree(dev);
	}

	usb_set_intfdata(intf, NULL);
}