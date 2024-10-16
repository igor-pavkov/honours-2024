static int technisat_usb2_set_led_timer(struct dvb_usb_device *d, u8 red, u8 green)
{
	struct technisat_usb2_state *state = d->priv;
	u8 *b = state->buf;
	int ret;

	b[0] = 0;

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	ret = usb_control_msg(d->udev, usb_sndctrlpipe(d->udev, 0),
		SET_LED_TIMER_DIVIDER_VENDOR_REQUEST,
		USB_TYPE_VENDOR | USB_DIR_OUT,
		(red << 8) | green, 0,
		b, 1, 500);

	mutex_unlock(&d->i2c_mutex);

	return ret;
}