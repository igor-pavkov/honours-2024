static int technisat_usb2_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	struct dvb_usb_device *dev;

	if (dvb_usb_device_init(intf, &technisat_usb2_devices, THIS_MODULE,
				&dev, adapter_nr) != 0)
		return -ENODEV;

	if (dev) {
		struct technisat_usb2_state *state = dev->priv;
		state->dev = dev;

		if (!disable_led_control) {
			INIT_DELAYED_WORK(&state->green_led_work,
					technisat_usb2_green_led_control);
			schedule_delayed_work(&state->green_led_work,
					msecs_to_jiffies(500));
		}
	}

	return 0;
}