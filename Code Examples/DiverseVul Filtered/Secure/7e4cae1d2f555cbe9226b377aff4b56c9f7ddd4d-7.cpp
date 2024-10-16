BN_ULONG bn_sub_part_words(BN_ULONG *r,
	const BN_ULONG *a, const BN_ULONG *b,
	int cl, int dl)
	{
	BN_ULONG c, t;

	assert(cl >= 0);
	c = bn_sub_words(r, a, b, cl);

	if (dl == 0)
		return c;

	r += cl;
	a += cl;
	b += cl;

	if (dl < 0)
		{
#ifdef BN_COUNT
		fprintf(stderr, "  bn_sub_part_words %d + %d (dl < 0, c = %d)\n", cl, dl, c);
#endif
		for (;;)
			{
			t = b[0];
			r[0] = (0-t-c)&BN_MASK2;
			if (t != 0) c=1;
			if (++dl >= 0) break;

			t = b[1];
			r[1] = (0-t-c)&BN_MASK2;
			if (t != 0) c=1;
			if (++dl >= 0) break;

			t = b[2];
			r[2] = (0-t-c)&BN_MASK2;
			if (t != 0) c=1;
			if (++dl >= 0) break;

			t = b[3];
			r[3] = (0-t-c)&BN_MASK2;
			if (t != 0) c=1;
			if (++dl >= 0) break;

			b += 4;
			r += 4;
			}
		}
	else
		{
		int save_dl = dl;
#ifdef BN_COUNT
		fprintf(stderr, "  bn_sub_part_words %d + %d (dl > 0, c = %d)\n", cl, dl, c);
#endif
		while(c)
			{
			t = a[0];
			r[0] = (t-c)&BN_MASK2;
			if (t != 0) c=0;
			if (--dl <= 0) break;

			t = a[1];
			r[1] = (t-c)&BN_MASK2;
			if (t != 0) c=0;
			if (--dl <= 0) break;

			t = a[2];
			r[2] = (t-c)&BN_MASK2;
			if (t != 0) c=0;
			if (--dl <= 0) break;

			t = a[3];
			r[3] = (t-c)&BN_MASK2;
			if (t != 0) c=0;
			if (--dl <= 0) break;

			save_dl = dl;
			a += 4;
			r += 4;
			}
		if (dl > 0)
			{
#ifdef BN_COUNT
			fprintf(stderr, "  bn_sub_part_words %d + %d (dl > 0, c == 0)\n", cl, dl);
#endif
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
			fprintf(stderr, "  bn_sub_part_words %d + %d (dl > 0, copy)\n", cl, dl);
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