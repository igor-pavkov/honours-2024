static void k_lock(struct vc_data *vc, unsigned char value, char up_flag)
{
	if (up_flag || rep)
		return;

	chg_vc_kbd_lock(kbd, value);
}