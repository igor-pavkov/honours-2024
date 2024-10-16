static void do_compute_shiftstate(void)
{
	unsigned int k, sym, val;

	shift_state = 0;
	memset(shift_down, 0, sizeof(shift_down));

	for_each_set_bit(k, key_down, min(NR_KEYS, KEY_CNT)) {
		sym = U(key_maps[0][k]);
		if (KTYP(sym) != KT_SHIFT && KTYP(sym) != KT_SLOCK)
			continue;

		val = KVAL(sym);
		if (val == KVAL(K_CAPSSHIFT))
			val = KVAL(K_SHIFT);

		shift_down[val]++;
		shift_state |= BIT(val);
	}
}