int vt_get_kbd_mode_bit(int console, int bit)
{
	struct kbd_struct *kb = kbd_table + console;
	return vc_kbd_mode(kb, bit);
}