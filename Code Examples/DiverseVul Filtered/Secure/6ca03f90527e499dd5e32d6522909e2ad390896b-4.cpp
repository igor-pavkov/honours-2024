static void kbd_event(struct input_handle *handle, unsigned int event_type,
		      unsigned int event_code, int value)
{
	/* We are called with interrupts disabled, just take the lock */
	spin_lock(&kbd_event_lock);

	if (event_type == EV_MSC && event_code == MSC_RAW && HW_RAW(handle->dev))
		kbd_rawcode(value);
	if (event_type == EV_KEY && event_code <= KEY_MAX)
		kbd_keycode(event_code, value, HW_RAW(handle->dev));

	spin_unlock(&kbd_event_lock);

	tasklet_schedule(&keyboard_tasklet);
	do_poke_blanked_console = 1;
	schedule_console_callback();
}