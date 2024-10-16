void setledstate(struct kbd_struct *kb, unsigned int led)
{
        unsigned long flags;
        spin_lock_irqsave(&led_lock, flags);
	if (!(led & ~7)) {
		ledioctl = led;
		kb->ledmode = LED_SHOW_IOCTL;
	} else
		kb->ledmode = LED_SHOW_FLAGS;

	set_leds();
	spin_unlock_irqrestore(&led_lock, flags);
}