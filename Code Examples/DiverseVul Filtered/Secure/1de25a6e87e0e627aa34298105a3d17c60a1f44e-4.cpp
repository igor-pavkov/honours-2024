static int inflate_get_next_window(STATE_PARAM_ONLY)
{
	gunzip_outbuf_count = 0;

	while (1) {
		int ret;

		if (need_another_block) {
			if (end_reached) {
				calculate_gunzip_crc(PASS_STATE_ONLY);
				end_reached = 0;
				/* NB: need_another_block is still set */
				return 0; /* Last block */
			}
			method = inflate_block(PASS_STATE &end_reached);
			need_another_block = 0;
		}

		switch (method) {
		case -1:
			ret = inflate_stored(PASS_STATE_ONLY);
			break;
		case -2:
			ret = inflate_codes(PASS_STATE_ONLY);
			break;
		default: /* cannot happen */
			abort_unzip(PASS_STATE_ONLY);
		}

		if (ret == 1) {
			calculate_gunzip_crc(PASS_STATE_ONLY);
			return 1; /* more data left */
		}
		need_another_block = 1; /* end of that block */
	}
	/* Doesnt get here */
}