BN_ULONG bn_add_part_words(BN_ULONG *r,
	const BN_ULONG *a, const BN_ULONG *b,
	int cl, int dl)
	{
	BN_ULONG c, l, t;

	assert(cl >= 0);
	c = bn_add_words(r, a, b, cl);

	if (dl == 0)
		return c;

	r += cl;
	a += cl;
	b += cl;

	if (dl < 0)
		{
		int save_dl = dl;
#ifdef BN_COUNT
		fprintf(stderr, "  bn_add_part_words %d + %d (dl < 0, c = %d)\n", cl, dl, c);
#endif
		while (c)
			{
			l=(c+b[0])&BN_MASK2;
			c=(l < c);
			r[0]=l;
			if (++dl >= 0) break;

			l=(c+b[1])&BN_MASK2;
			c=(l < c);
			r[1]=l;
			if (++dl >= 0) break;

			l=(c+b[2])&BN_MASK2;
			c=(l < c);
			r[2]=l;
			if (++dl >= 0) break;

			l=(c+b[3])&BN_MASK2;
			c=(l < c);
			r[3]=l;
			if (++dl >= 0) break;

			save_dl = dl;
			b+=4;
			r+=4;
			}
		if (dl < 0)
			{
#ifdef BN_COUNT
			fprintf(stderr, "  bn_add_part_words %d + %d (dl < 0, c == 0)\n", cl, dl);
#endif
			if (save_dl < dl)
				{
				switch (dl - save_dl)
					{
				case 1:
					r[1] = b[1];
					if (++dl >= 0) break;
				case 2:
					r[2] = b[2];
					if (++dl >= 0) break;
				case 3:
					r[3] = b[3];
					if (++dl >= 0) break;
					}
				b += 4;
				r += 4;
				}
			}
		if (dl < 0)
			{
#ifdef BN_COUNT
			fprintf(stderr, "  bn_add_part_words %d + %d (dl < 0, copy)\n", cl, dl);
#endif
			for(;;)
				{
				r[0] = b[0];
				if (++dl >= 0) break;
				r[1] = b[1];
				if (++dl >= 0) break;
				r[2] = b[2];
				if (++dl >= 0) break;
				r[3] = b[3];
				if (++dl >= 0) break;

				b += 4;
				r += 4;
				}
			}
		}
	else
		{
		int save_dl = dl;
#ifdef BN_COUNT
		fprintf(stderr, "  bn_add_part_words %d + %d (dl > 0)\n", cl, dl);
#endif
		while (c)
			{
			t=(a[0]+c)&BN_MASK2;
			c=(t < c);
			r[0]=t;
			if (--dl <= 0) break;

			t=(a[1]+c)&BN_MASK2;
			c=(t < c);
			r[1]=t;
			if (--dl <= 0) break;

			t=(a[2]+c)&BN_MASK2;
			c=(t < c);
			r[2]=t;
			if (--dl <= 0) break;

			t=(a[3]+c)&BN_MASK2;
			c=(t < c);
			r[3]=t;
			if (--dl <= 0) break;

			save_dl = dl;
			a+=4;
			r+=4;
			}
#ifdef BN_COUNT
		fprintf(stderr, "  bn_add_part_words %d + %d (dl > 0, c == 0)\n", cl, dl);
#endif
		if (dl > 0)
			{
			if (save_dl > dl)
				{
				switch (save_dl - dl)
					{
				case 1:
					r[1] = a[1];
					if (--dl <= 0) break;
				case 2:
					r[2] = a[2];
					if (--dl <= 0) break;
				case 3:
					r[3] = a[3];
					if (--dl <= 0) break;
					}
				a += 4;
				r += 4;
				}
			}
		if (dl > 0)
			{
#ifdef BN_COUNT
			fprintf(stderr, "  bn_add_part_words %d + %d (dl > 0, copy)\n", cl, dl);
#endif
			for(;;)
				{
				r[0] = a[0];
				if (--dl <= 0) break;
				r[1] = a[1];
				if (--dl <= 0) break;
				r[2] = a[2];
				if (--dl <= 0) break;
				r[3] = a[3];
				if (--dl <= 0) break;

				a += 4;
				r += 4;
				}
			}
		}
	return c;
	}