static void fn_enter(struct vc_data *vc)
{
	if (diacr) {
		if (kbd->kbdmode == VC_UNICODE)
			to_utf8(vc, diacr);
		else {
			int c = conv_uni_to_8bit(diacr);
			if (c != -1)
				put_queue(vc, c);
		}
		diacr = 0;
	}

	put_queue(vc, 13);
	if (vc_kbd_mode(kbd, VC_CRLF))
		put_queue(vc, 10);
}