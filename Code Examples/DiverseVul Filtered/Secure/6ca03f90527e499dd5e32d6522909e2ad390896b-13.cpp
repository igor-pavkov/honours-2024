static int kbd_update_leds_helper(struct input_handle *handle, void *data)
{
	unsigned int leds = *(unsigned int *)data;

	if (test_bit(EV_LED, handle->dev->evbit)) {
		input_inject_event(handle, EV_LED, LED_SCROLLL, !!(leds & 0x01));
		input_inject_event(handle, EV_LED, LED_NUML,    !!(leds & 0x02));
		input_inject_event(handle, EV_LED, LED_CAPSL,   !!(leds & 0x04));
		input_inject_event(handle, EV_SYN, SYN_REPORT, 0);
	}

	return 0;
}