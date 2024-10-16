static int inflate_stored(STATE_PARAM_ONLY)
{
	/* read and output the compressed data */
	while (inflate_stored_n--) {
		inflate_stored_b = fill_bitbuffer(PASS_STATE inflate_stored_b, &inflate_stored_k, 8);
		gunzip_window[inflate_stored_w++] = (unsigned char) inflate_stored_b;
		if (inflate_stored_w == GUNZIP_WSIZE) {
			gunzip_outbuf_count = inflate_stored_w;
			//flush_gunzip_window();
			inflate_stored_w = 0;
			inflate_stored_b >>= 8;
			inflate_stored_k -= 8;
			return 1; /* We have a block */
		}
		inflate_stored_b >>= 8;
		inflate_stored_k -= 8;
	}

	/* restore the globals from the locals */
	gunzip_outbuf_count = inflate_stored_w;		/* restore global gunzip_window pointer */
	gunzip_bb = inflate_stored_b;	/* restore global bit buffer */
	gunzip_bk = inflate_stored_k;
	return 0; /* Finished */
}