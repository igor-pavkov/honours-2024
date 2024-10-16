static int uvc_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct uvc_device *dev;
	int ret;

	if (id->idVendor && id->idProduct)
		uvc_trace(UVC_TRACE_PROBE, "Probing known UVC device %s "
				"(%04x:%04x)\n", udev->devpath, id->idVendor,
				id->idProduct);
	else
		uvc_trace(UVC_TRACE_PROBE, "Probing generic UVC device %s\n",
				udev->devpath);

	/* Allocate memory for the device and initialize it */
	if ((dev = kzalloc(sizeof *dev, GFP_KERNEL)) == NULL)
		return -ENOMEM;

	INIT_LIST_HEAD(&dev->entities);
	INIT_LIST_HEAD(&dev->streaming);
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(udev);
	dev->intf = usb_get_intf(intf);
	dev->intfnum = intf->cur_altsetting->desc.bInterfaceNumber;
	dev->quirks = id->driver_info | uvc_quirks_param;

	if (udev->product != NULL)
		strncpy(dev->name, udev->product, sizeof dev->name);
	else
		snprintf(dev->name, sizeof dev->name,
			"UVC Camera (%04x:%04x)",
			le16_to_cpu(udev->descriptor.idVendor),
			le16_to_cpu(udev->descriptor.idProduct));

	/* Parse the Video Class control descriptor */
	if (uvc_parse_control(dev) < 0) {
		uvc_trace(UVC_TRACE_PROBE, "Unable to parse UVC "
			"descriptors.\n");
		goto error;
	}

	uvc_printk(KERN_INFO, "Found UVC %u.%02u device %s (%04x:%04x)\n",
		dev->uvc_version >> 8, dev->uvc_version & 0xff,
		udev->product ? udev->product : "<unnamed>",
		le16_to_cpu(udev->descriptor.idVendor),
		le16_to_cpu(udev->descriptor.idProduct));

	if (uvc_quirks_param != 0) {
		uvc_printk(KERN_INFO, "Forcing device quirks 0x%x by module "
			"parameter for testing purpose.\n", uvc_quirks_param);
		uvc_printk(KERN_INFO, "Please report required quirks to the "
			"linux-uvc-devel mailing list.\n");
	}

	/* Initialize controls */
	if (uvc_ctrl_init_device(dev) < 0)
		goto error;

	/* Register the video devices */
	if (uvc_register_video(dev) < 0)
		goto error;

	/* Save our data pointer in the interface data */
	usb_set_intfdata(intf, dev);

	/* Initialize the interrupt URB */
	if ((ret = uvc_status_init(dev)) < 0) {
		uvc_printk(KERN_INFO, "Unable to initialize the status "
			"endpoint (%d), status interrupt will not be "
			"supported.\n", ret);
	}

	uvc_trace(UVC_TRACE_PROBE, "UVC device initialized.\n");
	return 0;

error:
	kref_put(&dev->kref, uvc_delete);
	return -ENODEV;
}