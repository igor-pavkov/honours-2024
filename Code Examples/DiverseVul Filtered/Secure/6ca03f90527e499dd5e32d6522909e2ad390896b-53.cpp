void vt_reset_keyboard(int console)
{
	struct kbd_struct *kb = kbd_table + console;
	unsigned long flags;

	spin_lock_irqsave(&kbd_event_lock, flags);
	set_vc_kbd_mode(kb, VC_REPEAT);
	clr_vc_kbd_mode(kb, VC_CKMODE);
	clr_vc_kbd_mode(kb, VC_APPLIC);
	clr_vc_kbd_mode(kb, VC_CRLF);
	kb->lockstate = 0;
	kb->slockstate = 0;
	spin_lock(&led_lock);
	kb->ledmode = LED_SHOW_FLAGS;
	kb->ledflagstate = kb->default_ledflagstate;
	spin_unlock(&led_lock);
	/* do not do set_leds here because this causes an endless tasklet loop
	   when the keyboard hasn't been initialized yet */
	spin_unlock_irqrestore(&kbd_event_lock, flags);
}