static int gadgetfs_bind(struct usb_gadget *gadget,
		struct usb_gadget_driver *driver)
{
	struct dev_data		*dev = the_device;

	if (!dev)
		return -ESRCH;
	if (0 != strcmp (CHIP, gadget->name)) {
		pr_err("%s expected %s controller not %s\n",
			shortname, CHIP, gadget->name);
		return -ENODEV;
	}

	set_gadget_data (gadget, dev);
	dev->gadget = gadget;
	gadget->ep0->driver_data = dev;

	/* preallocate control response and buffer */
	dev->req = usb_ep_alloc_request (gadget->ep0, GFP_KERNEL);
	if (!dev->req)
		goto enomem;
	dev->req->context = NULL;
	dev->req->complete = epio_complete;

	if (activate_ep_files (dev) < 0)
		goto enomem;

	INFO (dev, "bound to %s driver\n", gadget->name);
	spin_lock_irq(&dev->lock);
	dev->state = STATE_DEV_UNCONNECTED;
	spin_unlock_irq(&dev->lock);
	get_dev (dev);
	return 0;

enomem:
	gadgetfs_unbind (gadget);
	return -ENOMEM;
}