int dvb_usb_device_init(struct usb_interface *intf,
			struct dvb_usb_device_properties *props,
			struct module *owner, struct dvb_usb_device **du,
			short *adapter_nums)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct dvb_usb_device *d = NULL;
	struct dvb_usb_device_description *desc = NULL;

	int ret = -ENOMEM, cold = 0;

	if (du != NULL)
		*du = NULL;

	if ((desc = dvb_usb_find_device(udev, props, &cold)) == NULL) {
		deb_err("something went very wrong, device was not found in current device list - let's see what comes next.\n");
		return -ENODEV;
	}

	if (cold) {
		info("found a '%s' in cold state, will try to load a firmware", desc->name);
		ret = dvb_usb_download_firmware(udev, props);
		if (!props->no_reconnect || ret != 0)
			return ret;
	}

	info("found a '%s' in warm state.", desc->name);
	d = kzalloc(sizeof(struct dvb_usb_device), GFP_KERNEL);
	if (d == NULL) {
		err("no memory for 'struct dvb_usb_device'");
		return -ENOMEM;
	}

	d->udev = udev;
	memcpy(&d->props, props, sizeof(struct dvb_usb_device_properties));
	d->desc = desc;
	d->owner = owner;

	usb_set_intfdata(intf, d);

	if (du != NULL)
		*du = d;

	ret = dvb_usb_init(d, adapter_nums);

	if (ret == 0)
		info("%s successfully initialized and connected.", desc->name);
	else
		info("%s error while loading driver (%d)", desc->name, ret);
	return ret;
}