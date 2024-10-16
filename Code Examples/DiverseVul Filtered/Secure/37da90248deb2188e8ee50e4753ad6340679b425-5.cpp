static Bigint *lshift(Bigint *b, int k)
{
	int i, k1, n, n1;
	Bigint *b1;
	ULong *x, *x1, *xe, z;

#ifdef Pack_32
	n = k >> 5;
#else
	n = k >> 4;
#endif
	k1 = b->k;
	n1 = n + b->wds + 1;
	for(i = b->maxwds; n1 > i; i <<= 1) {
		k1++;
	}
	b1 = Balloc(k1);
	x1 = b1->x;
	for(i = 0; i < n; i++) {
		*x1++ = 0;
	}
	x = b->x;
	xe = x + b->wds;
#ifdef Pack_32
	if (k &= 0x1f) {
		k1 = 32 - k;
		z = 0;
		do {
			*x1++ = *x << k | z;
			z = *x++ >> k1;
		}
		while(x < xe);
		if ((*x1 = z)) {
			++n1;
		}
	}
#else
	if (k &= 0xf) {
		k1 = 16 - k;
		z = 0;
		do {
			*x1++ = *x << k  & 0xffff | z;
			z = *x++ >> k1;
		}
		while(x < xe);
		if (*x1 = z) {
			++n1;
		}
	}
#endif
	else do
		*x1++ = *x++;
	while(x < xe);
	b1->wds = n1 - 1;
	Bfree(b);
	return b1;
}