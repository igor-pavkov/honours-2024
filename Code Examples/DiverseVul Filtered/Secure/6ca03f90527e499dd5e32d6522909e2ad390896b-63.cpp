int vt_do_kdskbmeta(int console, unsigned int arg)
{
	struct kbd_struct *kb = kbd_table + console;
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&kbd_event_lock, flags);
	switch(arg) {
	case K_METABIT:
		clr_vc_kbd_mode(kb, VC_META);
		break;
	case K_ESCPREFIX:
		set_vc_kbd_mode(kb, VC_META);
		break;
	default:
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&kbd_event_lock, flags);
	return ret;
}