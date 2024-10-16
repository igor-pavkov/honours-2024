static void kbd_init_leds(void)
{
	int error;
	int i;

	for (i = 0; i < ARRAY_SIZE(kbd_led_triggers); i++) {
		error = led_trigger_register(&kbd_led_triggers[i].trigger);
		if (error)
			pr_err("error %d while registering trigger %s\n",
			       error, kbd_led_triggers[i].trigger.name);
	}
}