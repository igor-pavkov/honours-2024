static void kbd_propagate_led_state(unsigned int old_state,
				    unsigned int new_state)
{
	struct kbd_led_trigger *trigger;
	unsigned int changed = old_state ^ new_state;
	int i;

	for (i = 0; i < ARRAY_SIZE(kbd_led_triggers); i++) {
		trigger = &kbd_led_triggers[i];

		if (changed & trigger->mask)
			led_trigger_event(&trigger->trigger,
					  new_state & trigger->mask ?
						LED_FULL : LED_OFF);
	}
}