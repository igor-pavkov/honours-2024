static void k_cur(struct vc_data *vc, unsigned char value, char up_flag)
{
	static const char cur_chars[] = "BDCA";

	if (up_flag)
		return;

	applkey(vc, cur_chars[value], vc_kbd_mode(kbd, VC_CKMODE));
}