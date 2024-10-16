static void kbd_bh(unsigned long dummy)
{
	unsigned int leds;
	unsigned long flags;

	spin_lock_irqsave(&led_lock, flags);
	leds = getleds();
	leds |= (unsigned int)kbd->lockstate << 8;
	spin_unlock_irqrestore(&led_lock, flags);

	if (leds != ledstate) {
		kbd_propagate_led_state(ledstate, leds);
		ledstate = leds;
	}
}