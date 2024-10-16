void vt_set_led_state(int console, int leds)
{
	struct kbd_struct *kb = kbd_table + console;
	setledstate(kb, leds);
}