static void k_pad(struct vc_data *vc, unsigned char value, char up_flag)
{
	static const char pad_chars[] = "0123456789+-*/\015,.?()#";
	static const char app_map[] = "pqrstuvwxylSRQMnnmPQS";

	if (up_flag)
		return;		/* no action, if this is a key release */

	/* kludge... shift forces cursor/number keys */
	if (vc_kbd_mode(kbd, VC_APPLIC) && !shift_down[KG_SHIFT]) {
		applkey(vc, app_map[value], 1);
		return;
	}

	if (!vc_kbd_led(kbd, VC_NUMLOCK)) {

		switch (value) {
		case KVAL(K_PCOMMA):
		case KVAL(K_PDOT):
			k_fn(vc, KVAL(K_REMOVE), 0);
			return;
		case KVAL(K_P0):
			k_fn(vc, KVAL(K_INSERT), 0);
			return;
		case KVAL(K_P1):
			k_fn(vc, KVAL(K_SELECT), 0);
			return;
		case KVAL(K_P2):
			k_cur(vc, KVAL(K_DOWN), 0);
			return;
		case KVAL(K_P3):
			k_fn(vc, KVAL(K_PGDN), 0);
			return;
		case KVAL(K_P4):
			k_cur(vc, KVAL(K_LEFT), 0);
			return;
		case KVAL(K_P6):
			k_cur(vc, KVAL(K_RIGHT), 0);
			return;
		case KVAL(K_P7):
			k_fn(vc, KVAL(K_FIND), 0);
			return;
		case KVAL(K_P8):
			k_cur(vc, KVAL(K_UP), 0);
			return;
		case KVAL(K_P9):
			k_fn(vc, KVAL(K_PGUP), 0);
			return;
		case KVAL(K_P5):
			applkey(vc, 'G', vc_kbd_mode(kbd, VC_APPLIC));
			return;
		}
	}

	put_queue(vc, pad_chars[value]);
	if (value == KVAL(K_PENTER) && vc_kbd_mode(kbd, VC_CRLF))
		put_queue(vc, 10);
}