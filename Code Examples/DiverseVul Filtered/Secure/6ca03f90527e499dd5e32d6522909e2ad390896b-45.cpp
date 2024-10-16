int vt_do_kdgkbmeta(int console)
{
	struct kbd_struct *kb = kbd_table + console;
        /* Again a spot read so no locking */
	return vc_kbd_mode(kb, VC_META) ? K_ESCPREFIX : K_METABIT;
}