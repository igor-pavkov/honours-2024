static void kbd_propagate_led_state(unsigned int old_state,
				    unsigned int new_state)
{
	input_handler_for_each_handle(&kbd_handler, &new_state,
				      kbd_update_leds_helper);
}