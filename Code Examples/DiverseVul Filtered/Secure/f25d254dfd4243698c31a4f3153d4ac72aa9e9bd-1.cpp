static huft_t* huft_build(const unsigned *b, const unsigned n,
			const unsigned s, const struct cp_ext *cp_ext,
			unsigned *m)
{
	unsigned a;             /* counter for codes of length k */
	unsigned c[BMAX + 1];   /* bit length count table */
	unsigned eob_len;       /* length of end-of-block code (value 256) */
	unsigned f;             /* i repeats in table every f entries */
	int g;                  /* maximum code length */
	int htl;                /* table level */
	unsigned i;             /* counter, current code */
	unsigned j;             /* counter */
	int k;                  /* number of bits in current code */
	const unsigned *p;      /* pointer into c[], b[], or v[] */
	huft_t *q;              /* points to current table */
	huft_t r;               /* table entry for structure assignment */
	huft_t *u[BMAX];        /* table stack */
	unsigned v[N_MAX + 1];  /* values in order of bit length. last v[] is never used */
	int ws[BMAX + 1];       /* bits decoded stack */
	int w;                  /* bits decoded */
	unsigned x[BMAX + 1];   /* bit offsets, then code stack */
	unsigned *xp;           /* pointer into x */
	int y;                  /* number of dummy codes added */
	unsigned z;             /* number of entries in current table */
	huft_t *result;
	huft_t **t;

	/* Length of EOB code, if any */
	eob_len = n > 256 ? b[256] : BMAX;

	/* Generate counts for each bit length */
	memset(c, 0, sizeof(c));
	p = b;
	i = n;
	do {
		c[*p]++; /* assume all entries <= BMAX */
		p++;     /* can't combine with above line (Solaris bug) */
	} while (--i);
	if (c[0] == n) {  /* null input - all zero length codes */
		q = xzalloc(3 * sizeof(*q));
		//q[0].v.t = NULL;
		q[1].e = 99;    /* invalid code marker */
		q[1].b = 1;
		q[2].e = 99;    /* invalid code marker */
		q[2].b = 1;
		*m = 1;
		return q + 1;
	}

	/* Find minimum and maximum length, bound *m by those */
	for (j = 1; (j <= BMAX) && (c[j] == 0); j++)
		continue;
	k = j; /* minimum code length */
	for (i = BMAX; (c[i] == 0) && i; i--)
		continue;
	g = i; /* maximum code length */
	*m = (*m < j) ? j : ((*m > i) ? i : *m);

	/* Adjust last length count to fill out codes, if needed */
	for (y = 1 << j; j < i; j++, y <<= 1) {
		y -= c[j];
		if (y < 0)
			return ERR_RET; /* bad input: more codes than bits */
	}
	y -= c[i];
	if (y < 0)
		return ERR_RET;
	c[i] += y;

	/* Generate starting offsets into the value table for each length */
	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	while (--i) { /* note that i == g from above */
		j += *p++;
		*xp++ = j;
	}

	/* Make a table of values in order of bit lengths.
	 * To detect bad input, unused v[i]'s are set to invalid value UINT_MAX.
	 * In particular, last v[i] is never filled and must not be accessed.
	 */
	memset(v, 0xff, sizeof(v));
	p = b;
	i = 0;
	do {
		j = *p++;
		if (j != 0) {
			v[x[j]++] = i;
		}
	} while (++i < n);

	/* Generate the Huffman codes and for each, make the table entries */
	result = ERR_RET;
	t = &result;
	x[0] = i = 0;   /* first Huffman code is zero */
	p = v;          /* grab values in bit order */
	htl = -1;       /* no tables yet--level -1 */
	w = ws[0] = 0;  /* bits decoded */
	u[0] = NULL;    /* just to keep compilers happy */
	q = NULL;       /* ditto */
	z = 0;          /* ditto */

	/* go through the bit lengths (k already is bits in shortest code) */
	for (; k <= g; k++) {
		a = c[k];
		while (a--) {
			/* here i is the Huffman code of length k bits for value *p */
			/* make tables up to required level */
			while (k > ws[htl + 1]) {
				w = ws[++htl];

				/* compute minimum size table less than or equal to *m bits */
				z = g - w;
				z = z > *m ? *m : z; /* upper limit on table size */
				j = k - w;
				f = 1 << j;
				if (f > a + 1) { /* try a k-w bit table */
					/* too few codes for k-w bit table */
					f -= a + 1; /* deduct codes from patterns left */
					xp = c + k;
					while (++j < z) { /* try smaller tables up to z bits */
						f <<= 1;
						if (f <= *++xp) {
							break; /* enough codes to use up j bits */
						}
						f -= *xp; /* else deduct codes from patterns */
					}
				}
				j = (w + j > eob_len && w < eob_len) ? eob_len - w : j;	/* make EOB code end at table */
				z = 1 << j;	/* table entries for j-bit table */
				ws[htl+1] = w + j;	/* set bits decoded in stack */

				/* allocate and link in new table */
				q = xzalloc((z + 1) * sizeof(huft_t));
				*t = q + 1;	/* link to list for huft_free() */
				t = &(q->v.t);
				u[htl] = ++q;	/* table starts after link */

				/* connect to last table, if there is one */
				if (htl) {
					x[htl] = i; /* save pattern for backing up */
					r.b = (unsigned char) (w - ws[htl - 1]); /* bits to dump before this table */
					r.e = (unsigned char) (16 + j); /* bits in this table */
					r.v.t = q; /* pointer to this table */
					j = (i & ((1 << w) - 1)) >> ws[htl - 1];
					u[htl - 1][j] = r; /* connect to last table */
				}
			}

			/* set up table entry in r */
			r.b = (unsigned char) (k - w);
			if (/*p >= v + n || -- redundant, caught by the second check: */
			    *p == UINT_MAX /* do we access uninited v[i]? (see memset(v))*/
			) {
				r.e = 99; /* out of values--invalid code */
			} else if (*p < s) {
				r.e = (unsigned char) (*p < 256 ? 16 : 15);	/* 256 is EOB code */
				r.v.n = (unsigned short) (*p++); /* simple code is just the value */
			} else {
				r.e = (unsigned char) cp_ext->ext[*p - s]; /* non-simple--look up in lists */
				r.v.n = cp_ext->cp[*p++ - s];
			}

			/* fill code-like entries with r */
			f = 1 << (k - w);
			for (j = i >> w; j < z; j += f) {
				q[j] = r;
			}

			/* backwards increment the k-bit code i */
			for (j = 1 << (k - 1); i & j; j >>= 1) {
				i ^= j;
			}
			i ^= j;

			/* backup over finished tables */
			while ((i & ((1 << w) - 1)) != x[htl]) {
				w = ws[--htl];
			}
		}
	}

	/* return actual size of base table */
	*m = ws[1];

	if (y != 0 && g != 1) /* we were given an incomplete table */
		/* return "result" ORed with 1 */
		return (void*)((uintptr_t)result | 1);

	return result;
}