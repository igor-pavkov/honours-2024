static int technisat_usb2_identify_state(struct usb_device *udev,
		struct dvb_usb_device_properties *props,
		struct dvb_usb_device_description **desc, int *cold)
{
	int ret;
	u8 *version;

	version = kmalloc(3, GFP_KERNEL);
	if (!version)
		return -ENOMEM;

	/* first select the interface */
	if (usb_set_interface(udev, 0, 1) != 0)
		err("could not set alternate setting to 0");
	else
		info("set alternate setting");

	*cold = 0; /* by default do not download a firmware - just in case something is wrong */

	ret = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
		GET_VERSION_INFO_VENDOR_REQUEST,
		USB_TYPE_VENDOR | USB_DIR_IN,
		0, 0,
		version, 3, 500);

	if (ret < 0)
		*cold = 1;
	else {
		info("firmware version: %d.%d", version[1], version[2]);
		*cold = 0;
	}

	kfree(version);

	return 0;
}