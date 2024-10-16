static Bigint * mult(Bigint *a, Bigint *b)
{
	Bigint *c;
	int k, wa, wb, wc;
	ULong carry, y, z;
	ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
#ifdef Pack_32
	ULong z2;
#endif

	if (a->wds < b->wds) {
		c = a;
		a = b;
		b = c;
	}
	k = a->k;
	wa = a->wds;
	wb = b->wds;
	wc = wa + wb;
	if (wc > a->maxwds) {
		k++;
	}
	c = Balloc(k);
	for(x = c->x, xa = x + wc; x < xa; x++) {
		*x = 0;
	}
	xa = a->x;
	xae = xa + wa;
	xb = b->x;
	xbe = xb + wb;
	xc0 = c->x;
#ifdef Pack_32
	for(; xb < xbe; xb++, xc0++) {
		if ((y = *xb & 0xffff)) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
				carry = z >> 16;
				z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
				carry = z2 >> 16;
				Storeinc(xc, z2, z);
			}
			while(x < xae);
			*xc = carry;
		}
		if ((y = *xb >> 16)) {
			x = xa;
			xc = xc0;
			carry = 0;
			z2 = *xc;
			do {
				z = (*x & 0xffff) * y + (*xc >> 16) + carry;
				carry = z >> 16;
				Storeinc(xc, z, z2);
				z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
				carry = z2 >> 16;
			}
			while(x < xae);
			*xc = z2;
		}
	}
#else
	for(; xb < xbe; xc0++) {
		if (y = *xb++) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * y + *xc + carry;
				carry = z >> 16;
				*xc++ = z & 0xffff;
			}
			while(x < xae);
			*xc = carry;
		}
	}
#endif
	for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
	c->wds = wc;
	return c;
}