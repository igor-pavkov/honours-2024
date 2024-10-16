static NOINLINE int inflate_codes(STATE_PARAM_ONLY)
{
	unsigned e;	/* table entry flag/number of extra bits */
	huft_t *t;	/* pointer to table entry */

	if (resume_copy)
		goto do_copy;

	while (1) {			/* do until end of block */
		bb = fill_bitbuffer(PASS_STATE bb, &k, bl);
		t = tl + ((unsigned) bb & ml);
		e = t->e;
		if (e > 16)
			do {
				if (e == 99) {
					abort_unzip(PASS_STATE_ONLY);
				}
				bb >>= t->b;
				k -= t->b;
				e -= 16;
				bb = fill_bitbuffer(PASS_STATE bb, &k, e);
				t = t->v.t + ((unsigned) bb & mask_bits[e]);
				e = t->e;
			} while (e > 16);
		bb >>= t->b;
		k -= t->b;
		if (e == 16) {	/* then it's a literal */
			gunzip_window[w++] = (unsigned char) t->v.n;
			if (w == GUNZIP_WSIZE) {
				gunzip_outbuf_count = w;
				//flush_gunzip_window();
				w = 0;
				return 1; // We have a block to read
			}
		} else {		/* it's an EOB or a length */
			/* exit if end of block */
			if (e == 15) {
				break;
			}

			/* get length of block to copy */
			bb = fill_bitbuffer(PASS_STATE bb, &k, e);
			nn = t->v.n + ((unsigned) bb & mask_bits[e]);
			bb >>= e;
			k -= e;

			/* decode distance of block to copy */
			bb = fill_bitbuffer(PASS_STATE bb, &k, bd);
			t = td + ((unsigned) bb & md);
			e = t->e;
			if (e > 16)
				do {
					if (e == 99) {
						abort_unzip(PASS_STATE_ONLY);
					}
					bb >>= t->b;
					k -= t->b;
					e -= 16;
					bb = fill_bitbuffer(PASS_STATE bb, &k, e);
					t = t->v.t + ((unsigned) bb & mask_bits[e]);
					e = t->e;
				} while (e > 16);
			bb >>= t->b;
			k -= t->b;
			bb = fill_bitbuffer(PASS_STATE bb, &k, e);
			dd = w - t->v.n - ((unsigned) bb & mask_bits[e]);
			bb >>= e;
			k -= e;

			/* do the copy */
 do_copy:
			do {
				/* Was: nn -= (e = (e = GUNZIP_WSIZE - ((dd &= GUNZIP_WSIZE - 1) > w ? dd : w)) > nn ? nn : e); */
				/* Who wrote THAT?? rewritten as: */
				unsigned delta;

				dd &= GUNZIP_WSIZE - 1;
				e = GUNZIP_WSIZE - (dd > w ? dd : w);
				delta = w > dd ? w - dd : dd - w;
				if (e > nn) e = nn;
				nn -= e;

				/* copy to new buffer to prevent possible overwrite */
				if (delta >= e) {
					memcpy(gunzip_window + w, gunzip_window + dd, e);
					w += e;
					dd += e;
				} else {
					/* do it slow to avoid memcpy() overlap */
					/* !NOMEMCPY */
					do {
						gunzip_window[w++] = gunzip_window[dd++];
					} while (--e);
				}
				if (w == GUNZIP_WSIZE) {
					gunzip_outbuf_count = w;
					resume_copy = (nn != 0);
					//flush_gunzip_window();
					w = 0;
					return 1;
				}
			} while (nn);
			resume_copy = 0;
		}
	}

	/* restore the globals from the locals */
	gunzip_outbuf_count = w;	/* restore global gunzip_window pointer */
	gunzip_bb = bb;			/* restore global bit buffer */
	gunzip_bk = k;

	/* normally just after call to inflate_codes, but save code by putting it here */
	/* free the decoding tables (tl and td), return */
	huft_free_all(PASS_STATE_ONLY);

	/* done */
	return 0;
}