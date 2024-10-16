static void kbd_keycode(unsigned int keycode, int down, int hw_raw)
{
	struct vc_data *vc = vc_cons[fg_console].d;
	unsigned short keysym, *key_map;
	unsigned char type;
	bool raw_mode;
	struct tty_struct *tty;
	int shift_final;
	struct keyboard_notifier_param param = { .vc = vc, .value = keycode, .down = down };
	int rc;

	tty = vc->port.tty;

	if (tty && (!tty->driver_data)) {
		/* No driver data? Strange. Okay we fix it then. */
		tty->driver_data = vc;
	}

	kbd = kbd_table + vc->vc_num;

#ifdef CONFIG_SPARC
	if (keycode == KEY_STOP)
		sparc_l1_a_state = down;
#endif

	rep = (down == 2);

	raw_mode = (kbd->kbdmode == VC_RAW);
	if (raw_mode && !hw_raw)
		if (emulate_raw(vc, keycode, !down << 7))
			if (keycode < BTN_MISC && printk_ratelimit())
				pr_warn("can't emulate rawmode for keycode %d\n",
					keycode);

#ifdef CONFIG_SPARC
	if (keycode == KEY_A && sparc_l1_a_state) {
		sparc_l1_a_state = false;
		sun_do_break();
	}
#endif

	if (kbd->kbdmode == VC_MEDIUMRAW) {
		/*
		 * This is extended medium raw mode, with keys above 127
		 * encoded as 0, high 7 bits, low 7 bits, with the 0 bearing
		 * the 'up' flag if needed. 0 is reserved, so this shouldn't
		 * interfere with anything else. The two bytes after 0 will
		 * always have the up flag set not to interfere with older
		 * applications. This allows for 16384 different keycodes,
		 * which should be enough.
		 */
		if (keycode < 128) {
			put_queue(vc, keycode | (!down << 7));
		} else {
			put_queue(vc, !down << 7);
			put_queue(vc, (keycode >> 7) | 0x80);
			put_queue(vc, keycode | 0x80);
		}
		raw_mode = true;
	}

	if (down)
		set_bit(keycode, key_down);
	else
		clear_bit(keycode, key_down);

	if (rep &&
	    (!vc_kbd_mode(kbd, VC_REPEAT) ||
	     (tty && !L_ECHO(tty) && tty_chars_in_buffer(tty)))) {
		/*
		 * Don't repeat a key if the input buffers are not empty and the
		 * characters get aren't echoed locally. This makes key repeat
		 * usable with slow applications and under heavy loads.
		 */
		return;
	}

	param.shift = shift_final = (shift_state | kbd->slockstate) ^ kbd->lockstate;
	param.ledstate = kbd->ledflagstate;
	key_map = key_maps[shift_final];

	rc = atomic_notifier_call_chain(&keyboard_notifier_list,
					KBD_KEYCODE, &param);
	if (rc == NOTIFY_STOP || !key_map) {
		atomic_notifier_call_chain(&keyboard_notifier_list,
					   KBD_UNBOUND_KEYCODE, &param);
		do_compute_shiftstate();
		kbd->slockstate = 0;
		return;
	}

	if (keycode < NR_KEYS)
		keysym = key_map[keycode];
	else if (keycode >= KEY_BRL_DOT1 && keycode <= KEY_BRL_DOT8)
		keysym = U(K(KT_BRL, keycode - KEY_BRL_DOT1 + 1));
	else
		return;

	type = KTYP(keysym);

	if (type < 0xf0) {
		param.value = keysym;
		rc = atomic_notifier_call_chain(&keyboard_notifier_list,
						KBD_UNICODE, &param);
		if (rc != NOTIFY_STOP)
			if (down && !raw_mode)
				k_unicode(vc, keysym, !down);
		return;
	}

	type -= 0xf0;

	if (type == KT_LETTER) {
		type = KT_LATIN;
		if (vc_kbd_led(kbd, VC_CAPSLOCK)) {
			key_map = key_maps[shift_final ^ (1 << KG_SHIFT)];
			if (key_map)
				keysym = key_map[keycode];
		}
	}

	param.value = keysym;
	rc = atomic_notifier_call_chain(&keyboard_notifier_list,
					KBD_KEYSYM, &param);
	if (rc == NOTIFY_STOP)
		return;

	if ((raw_mode || kbd->kbdmode == VC_OFF) && type != KT_SPEC && type != KT_SHIFT)
		return;

	(*k_handler[type])(vc, keysym & 0xff, !down);

	param.ledstate = kbd->ledflagstate;
	atomic_notifier_call_chain(&keyboard_notifier_list, KBD_POST_KEYSYM, &param);

	if (type != KT_SLOCK)
		kbd->slockstate = 0;
}