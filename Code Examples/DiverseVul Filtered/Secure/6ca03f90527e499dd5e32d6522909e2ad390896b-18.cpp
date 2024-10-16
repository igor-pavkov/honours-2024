int vt_do_kdgkbmode(int console)
{
	struct kbd_struct *kb = kbd_table + console;
	/* This is a spot read so needs no locking */
	switch (kb->kbdmode) {
	case VC_RAW:
		return K_RAW;
	case VC_MEDIUMRAW:
		return K_MEDIUMRAW;
	case VC_UNICODE:
		return K_UNICODE;
	case VC_OFF:
		return K_OFF;
	default:
		return K_XLATE;
	}
}