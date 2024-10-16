static int em28xx_usb_suspend(struct usb_interface *intf,
			      pm_message_t message)
{
	struct em28xx *dev;

	dev = usb_get_intfdata(intf);
	if (!dev)
		return 0;
	em28xx_suspend_extension(dev);
	return 0;
}