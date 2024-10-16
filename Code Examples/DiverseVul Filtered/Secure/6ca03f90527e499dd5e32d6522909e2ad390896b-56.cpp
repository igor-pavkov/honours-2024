static void kbd_rawcode(unsigned char data)
{
	struct vc_data *vc = vc_cons[fg_console].d;

	kbd = kbd_table + vc->vc_num;
	if (kbd->kbdmode == VC_RAW)
		put_queue(vc, data);
}