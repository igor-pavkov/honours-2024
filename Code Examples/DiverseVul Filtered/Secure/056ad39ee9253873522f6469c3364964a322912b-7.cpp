int usb_set_isoch_delay(struct usb_device *dev)
{
	/* skip hub devices */
	if (dev->descriptor.bDeviceClass == USB_CLASS_HUB)
		return 0;

	/* skip non-SS/non-SSP devices */
	if (dev->speed < USB_SPEED_SUPER)
		return 0;

	return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
			USB_REQ_SET_ISOCH_DELAY,
			USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
			dev->hub_delay, 0, NULL, 0,
			USB_CTRL_SET_TIMEOUT);
}