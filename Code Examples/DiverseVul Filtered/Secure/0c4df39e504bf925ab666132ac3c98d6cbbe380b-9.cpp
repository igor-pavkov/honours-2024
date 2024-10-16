static int technisat_usb2_rc_query(struct dvb_usb_device *d)
{
	int ret = technisat_usb2_get_ir(d);

	if (ret < 0)
		return ret;

	if (ret == 0)
		return 0;

	if (!disable_led_control)
		technisat_usb2_set_led(d, 1, TECH_LED_BLINK);

	return 0;
}