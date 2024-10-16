void vt_kbd_con_start(int console)
{
	struct kbd_struct *kb = kbd_table + console;
	unsigned long flags;
	spin_lock_irqsave(&led_lock, flags);
	clr_vc_kbd_led(kb, VC_SCROLLOCK);
	set_leds();
	spin_unlock_irqrestore(&led_lock, flags);
}