static int emulate_raw(struct vc_data *vc, unsigned int keycode,
		       unsigned char up_flag)
{
	int code;

	switch (keycode) {

	case KEY_PAUSE:
		put_queue(vc, 0xe1);
		put_queue(vc, 0x1d | up_flag);
		put_queue(vc, 0x45 | up_flag);
		break;

	case KEY_HANGEUL:
		if (!up_flag)
			put_queue(vc, 0xf2);
		break;

	case KEY_HANJA:
		if (!up_flag)
			put_queue(vc, 0xf1);
		break;

	case KEY_SYSRQ:
		/*
		 * Real AT keyboards (that's what we're trying
		 * to emulate here) emit 0xe0 0x2a 0xe0 0x37 when
		 * pressing PrtSc/SysRq alone, but simply 0x54
		 * when pressing Alt+PrtSc/SysRq.
		 */
		if (test_bit(KEY_LEFTALT, key_down) ||
		    test_bit(KEY_RIGHTALT, key_down)) {
			put_queue(vc, 0x54 | up_flag);
		} else {
			put_queue(vc, 0xe0);
			put_queue(vc, 0x2a | up_flag);
			put_queue(vc, 0xe0);
			put_queue(vc, 0x37 | up_flag);
		}
		break;

	default:
		if (keycode > 255)
			return -1;

		code = x86_keycodes[keycode];
		if (!code)
			return -1;

		if (code & 0x100)
			put_queue(vc, 0xe0);
		put_queue(vc, (code & 0x7f) | up_flag);

		break;
	}

	return 0;
}