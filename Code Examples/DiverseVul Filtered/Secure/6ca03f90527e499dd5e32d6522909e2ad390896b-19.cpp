int vt_get_leds(int console, int flag)
{
	struct kbd_struct *kb = kbd_table + console;
	int ret;
	unsigned long flags;

	spin_lock_irqsave(&led_lock, flags);
	ret = vc_kbd_led(kb, flag);
	spin_unlock_irqrestore(&led_lock, flags);

	return ret;
}