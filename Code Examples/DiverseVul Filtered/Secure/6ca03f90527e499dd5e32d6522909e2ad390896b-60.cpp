static int kbd_led_trigger_activate(struct led_classdev *cdev)
{
	struct kbd_led_trigger *trigger =
		container_of(cdev->trigger, struct kbd_led_trigger, trigger);

	tasklet_disable(&keyboard_tasklet);
	if (ledstate != -1U)
		led_trigger_event(&trigger->trigger,
				  ledstate & trigger->mask ?
					LED_FULL : LED_OFF);
	tasklet_enable(&keyboard_tasklet);

	return 0;
}