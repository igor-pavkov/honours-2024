static void em28xx_usb_disconnect(struct usb_interface *intf)
{
	struct em28xx *dev;

	dev = usb_get_intfdata(intf);
	usb_set_intfdata(intf, NULL);

	if (!dev)
		return;

	if (dev->dev_next) {
		dev->dev_next->disconnected = 1;
		dev_info(&dev->intf->dev, "Disconnecting %s\n",
			 dev->dev_next->name);
	}

	dev->disconnected = 1;

	dev_info(&dev->intf->dev, "Disconnecting %s\n", dev->name);

	flush_request_modules(dev);

	em28xx_close_extension(dev);

	if (dev->dev_next) {
		em28xx_close_extension(dev->dev_next);
		em28xx_release_resources(dev->dev_next);
	}

	em28xx_release_resources(dev);

	if (dev->dev_next) {
		kref_put(&dev->dev_next->ref, em28xx_free_device);
		dev->dev_next = NULL;
	}
	kref_put(&dev->ref, em28xx_free_device);
}