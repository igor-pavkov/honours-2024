int vt_do_kdskbmode(int console, unsigned int arg)
{
	struct kbd_struct *kb = kbd_table + console;
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&kbd_event_lock, flags);
	switch(arg) {
	case K_RAW:
		kb->kbdmode = VC_RAW;
		break;
	case K_MEDIUMRAW:
		kb->kbdmode = VC_MEDIUMRAW;
		break;
	case K_XLATE:
		kb->kbdmode = VC_XLATE;
		do_compute_shiftstate();
		break;
	case K_UNICODE:
		kb->kbdmode = VC_UNICODE;
		do_compute_shiftstate();
		break;
	case K_OFF:
		kb->kbdmode = VC_OFF;
		break;
	default:
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&kbd_event_lock, flags);
	return ret;
}