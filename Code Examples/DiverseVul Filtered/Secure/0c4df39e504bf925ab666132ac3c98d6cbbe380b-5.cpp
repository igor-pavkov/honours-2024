static int technisat_usb2_set_led(struct dvb_usb_device *d, int red,
				  enum technisat_usb2_led_state st)
{
	struct technisat_usb2_state *state = d->priv;
	u8 *led = state->buf;
	int ret;

	led[0] = red ? SET_RED_LED_VENDOR_REQUEST : SET_GREEN_LED_VENDOR_REQUEST;

	if (disable_led_control && st != TECH_LED_OFF)
		return 0;

	switch (st) {
	case TECH_LED_ON:
		led[1] = 0x82;
		break;
	case TECH_LED_BLINK:
		led[1] = 0x82;
		if (red) {
			led[2] = 0x02;
			led[3] = 10;
			led[4] = 10;
		} else {
			led[2] = 0xff;
			led[3] = 50;
			led[4] = 50;
		}
		led[5] = 1;
		break;

	default:
	case TECH_LED_OFF:
		led[1] = 0x80;
		break;
	}

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	ret = usb_control_msg(d->udev, usb_sndctrlpipe(d->udev, 0),
		red ? SET_RED_LED_VENDOR_REQUEST : SET_GREEN_LED_VENDOR_REQUEST,
		USB_TYPE_VENDOR | USB_DIR_OUT,
		0, 0,
		led, 8, 500);

	mutex_unlock(&d->i2c_mutex);
	return ret;
}