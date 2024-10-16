static void kbd_start(struct input_handle *handle)
{
	tasklet_disable(&keyboard_tasklet);

	if (ledstate != -1U)
		kbd_update_leds_helper(handle, &ledstate);

	tasklet_enable(&keyboard_tasklet);
}