static void technisat_usb2_disconnect(struct usb_interface *intf)
{
	struct dvb_usb_device *dev = usb_get_intfdata(intf);

	/* work and stuff was only created when the device is is hot-state */
	if (dev != NULL) {
		struct technisat_usb2_state *state = dev->priv;
		if (state != NULL)
			cancel_delayed_work_sync(&state->green_led_work);
	}

	dvb_usb_device_exit(intf);
}