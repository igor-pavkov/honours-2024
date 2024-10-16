static int kbd_update_leds_helper(struct input_handle *handle, void *data)
{
	unsigned int led_state = *(unsigned int *)data;

	if (test_bit(EV_LED, handle->dev->evbit))
		kbd_propagate_led_state(~led_state, led_state);

	return 0;
}