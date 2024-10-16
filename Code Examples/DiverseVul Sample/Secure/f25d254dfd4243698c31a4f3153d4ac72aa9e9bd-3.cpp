static int inflate_block(STATE_PARAM smallint *e)
{
	unsigned ll[286 + 30];  /* literal/length and distance code lengths */
	unsigned t;     /* block type */
	unsigned b;     /* bit buffer */
	unsigned k;     /* number of bits in bit buffer */

	/* make local bit buffer */

	b = gunzip_bb;
	k = gunzip_bk;

	/* read in last block bit */
	b = fill_bitbuffer(PASS_STATE b, &k, 1);
	*e = b & 1;
	b >>= 1;
	k -= 1;

	/* read in block type */
	b = fill_bitbuffer(PASS_STATE b, &k, 2);
	t = (unsigned) b & 3;
	b >>= 2;
	k -= 2;

	/* restore the global bit buffer */
	gunzip_bb = b;
	gunzip_bk = k;

	/* Do we see block type 1 often? Yes!
	 * TODO: fix performance problem (see below) */
	//bb_error_msg("blktype %d", t);

	/* inflate that block type */
	switch (t) {
	case 0: /* Inflate stored */
	{
		unsigned n;	/* number of bytes in block */
		unsigned b_stored;	/* bit buffer */
		unsigned k_stored;	/* number of bits in bit buffer */

		/* make local copies of globals */
		b_stored = gunzip_bb;	/* initialize bit buffer */
		k_stored = gunzip_bk;

		/* go to byte boundary */
		n = k_stored & 7;
		b_stored >>= n;
		k_stored -= n;

		/* get the length and its complement */
		b_stored = fill_bitbuffer(PASS_STATE b_stored, &k_stored, 16);
		n = ((unsigned) b_stored & 0xffff);
		b_stored >>= 16;
		k_stored -= 16;

		b_stored = fill_bitbuffer(PASS_STATE b_stored, &k_stored, 16);
		if (n != (unsigned) ((~b_stored) & 0xffff)) {
			abort_unzip(PASS_STATE_ONLY);	/* error in compressed data */
		}
		b_stored >>= 16;
		k_stored -= 16;

		inflate_stored_setup(PASS_STATE n, b_stored, k_stored);

		return -1;
	}
	case 1:
	/* Inflate fixed
	 * decompress an inflated type 1 (fixed Huffman codes) block. We should
	 * either replace this with a custom decoder, or at least precompute the
	 * Huffman tables. TODO */
	{
		int i;                  /* temporary variable */
		unsigned bl;            /* lookup bits for tl */
		unsigned bd;            /* lookup bits for td */
		/* gcc 4.2.1 is too dumb to reuse stackspace. Moved up... */
		//unsigned ll[288];     /* length list for huft_build */

		/* set up literal table */
		for (i = 0; i < 144; i++)
			ll[i] = 8;
		for (; i < 256; i++)
			ll[i] = 9;
		for (; i < 280; i++)
			ll[i] = 7;
		for (; i < 288; i++) /* make a complete, but wrong code set */
			ll[i] = 8;
		bl = 7;
		inflate_codes_tl = huft_build(ll, 288, 257, &lit, &bl);
		/* ^^^ never returns error here - we use known data */

		/* set up distance table */
		for (i = 0; i < 30; i++) /* make an incomplete code set */
			ll[i] = 5;
		bd = 5;
		inflate_codes_td = huft_build(ll, 30, 0, &dist, &bd);
		/* ^^^ does return error here! (lsb bit is set) - we gave it incomplete code set */
		/* clearing error bit: */
		inflate_codes_td = (void*)((uintptr_t)inflate_codes_td & ~(uintptr_t)1);

		/* set up data for inflate_codes() */
		inflate_codes_setup(PASS_STATE bl, bd);

		/* huft_free code moved into inflate_codes */

		return -2;
	}
	case 2: /* Inflate dynamic */
	{
		enum { dbits = 6 };     /* bits in base distance lookup table */
		enum { lbits = 9 };     /* bits in base literal/length lookup table */

		huft_t *td;             /* distance code table */
		unsigned i;             /* temporary variables */
		unsigned j;
		unsigned l;             /* last length */
		unsigned m;             /* mask for bit lengths table */
		unsigned n;             /* number of lengths to get */
		unsigned bl;            /* lookup bits for tl */
		unsigned bd;            /* lookup bits for td */
		unsigned nb;            /* number of bit length codes */
		unsigned nl;            /* number of literal/length codes */
		unsigned nd;            /* number of distance codes */

		//unsigned ll[286 + 30];/* literal/length and distance code lengths */
		unsigned b_dynamic;     /* bit buffer */
		unsigned k_dynamic;     /* number of bits in bit buffer */

		/* make local bit buffer */
		b_dynamic = gunzip_bb;
		k_dynamic = gunzip_bk;

		/* read in table lengths */
		b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 5);
		nl = 257 + ((unsigned) b_dynamic & 0x1f);	/* number of literal/length codes */

		b_dynamic >>= 5;
		k_dynamic -= 5;
		b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 5);
		nd = 1 + ((unsigned) b_dynamic & 0x1f);	/* number of distance codes */

		b_dynamic >>= 5;
		k_dynamic -= 5;
		b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 4);
		nb = 4 + ((unsigned) b_dynamic & 0xf);	/* number of bit length codes */

		b_dynamic >>= 4;
		k_dynamic -= 4;
		if (nl > 286 || nd > 30) {
			abort_unzip(PASS_STATE_ONLY);	/* bad lengths */
		}

		/* read in bit-length-code lengths */
		for (j = 0; j < nb; j++) {
			b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 3);
			ll[border[j]] = (unsigned) b_dynamic & 7;
			b_dynamic >>= 3;
			k_dynamic -= 3;
		}
		for (; j < 19; j++)
			ll[border[j]] = 0;

		/* build decoding table for trees - single level, 7 bit lookup */
		bl = 7;
		inflate_codes_tl = huft_build(ll, 19, 19, NULL, &bl);
		if (BAD_HUFT(inflate_codes_tl)) {
			abort_unzip(PASS_STATE_ONLY);	/* incomplete code set */
		}

		/* read in literal and distance code lengths */
		n = nl + nd;
		m = mask_bits[bl];
		i = l = 0;
		while ((unsigned) i < n) {
			b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, (unsigned)bl);
			td = inflate_codes_tl + ((unsigned) b_dynamic & m);
			j = td->b;
			b_dynamic >>= j;
			k_dynamic -= j;
			j = td->v.n;
			if (j < 16) {	/* length of code in bits (0..15) */
				ll[i++] = l = j;	/* save last length in l */
			} else if (j == 16) {	/* repeat last length 3 to 6 times */
				b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 2);
				j = 3 + ((unsigned) b_dynamic & 3);
				b_dynamic >>= 2;
				k_dynamic -= 2;
				if ((unsigned) i + j > n) {
					abort_unzip(PASS_STATE_ONLY); //return 1;
				}
				while (j--) {
					ll[i++] = l;
				}
			} else if (j == 17) {	/* 3 to 10 zero length codes */
				b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 3);
				j = 3 + ((unsigned) b_dynamic & 7);
				b_dynamic >>= 3;
				k_dynamic -= 3;
				if ((unsigned) i + j > n) {
					abort_unzip(PASS_STATE_ONLY); //return 1;
				}
				while (j--) {
					ll[i++] = 0;
				}
				l = 0;
			} else {	/* j == 18: 11 to 138 zero length codes */
				b_dynamic = fill_bitbuffer(PASS_STATE b_dynamic, &k_dynamic, 7);
				j = 11 + ((unsigned) b_dynamic & 0x7f);
				b_dynamic >>= 7;
				k_dynamic -= 7;
				if ((unsigned) i + j > n) {
					abort_unzip(PASS_STATE_ONLY); //return 1;
				}
				while (j--) {
					ll[i++] = 0;
				}
				l = 0;
			}
		}

		/* free decoding table for trees */
		huft_free(inflate_codes_tl);

		/* restore the global bit buffer */
		gunzip_bb = b_dynamic;
		gunzip_bk = k_dynamic;

		/* build the decoding tables for literal/length and distance codes */
		bl = lbits;
		inflate_codes_tl = huft_build(ll, nl, 257, &lit, &bl);
		if (BAD_HUFT(inflate_codes_tl)) {
			abort_unzip(PASS_STATE_ONLY);
		}
		bd = dbits;
		inflate_codes_td = huft_build(ll + nl, nd, 0, &dist, &bd);
		if (BAD_HUFT(inflate_codes_td)) {
			abort_unzip(PASS_STATE_ONLY);
		}

		/* set up data for inflate_codes() */
		inflate_codes_setup(PASS_STATE bl, bd);

		/* huft_free code moved into inflate_codes */

		return -2;
	}
	default:
		abort_unzip(PASS_STATE_ONLY);
	}
}