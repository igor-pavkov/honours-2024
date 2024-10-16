static int cpia2_usb_probe(struct usb_interface *intf,
			   const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct usb_interface_descriptor *interface;
	struct camera_data *cam;
	int ret;

	/* A multi-config CPiA2 camera? */
	if (udev->descriptor.bNumConfigurations != 1)
		return -ENODEV;
	interface = &intf->cur_altsetting->desc;

	/* If we get to this point, we found a CPiA2 camera */
	LOG("CPiA2 USB camera found\n");

	cam = cpia2_init_camera_struct(intf);
	if (cam == NULL)
		return -ENOMEM;

	cam->dev = udev;
	cam->iface = interface->bInterfaceNumber;

	ret = set_alternate(cam, USBIF_CMDONLY);
	if (ret < 0) {
		ERR("%s: usb_set_interface error (ret = %d)\n", __func__, ret);
		kfree(cam);
		return ret;
	}


	if((ret = cpia2_init_camera(cam)) < 0) {
		ERR("%s: failed to initialize cpia2 camera (ret = %d)\n", __func__, ret);
		kfree(cam);
		return ret;
	}
	LOG("  CPiA Version: %d.%02d (%d.%d)\n",
	       cam->params.version.firmware_revision_hi,
	       cam->params.version.firmware_revision_lo,
	       cam->params.version.asic_id,
	       cam->params.version.asic_rev);
	LOG("  CPiA PnP-ID: %04x:%04x:%04x\n",
	       cam->params.pnp_id.vendor,
	       cam->params.pnp_id.product,
	       cam->params.pnp_id.device_revision);
	LOG("  SensorID: %d.(version %d)\n",
	       cam->params.version.sensor_flags,
	       cam->params.version.sensor_rev);

	usb_set_intfdata(intf, cam);

	ret = cpia2_register_camera(cam);
	if (ret < 0) {
		ERR("%s: Failed to register cpia2 camera (ret = %d)\n", __func__, ret);
		kfree(cam);
		return ret;
	}

	return 0;
}