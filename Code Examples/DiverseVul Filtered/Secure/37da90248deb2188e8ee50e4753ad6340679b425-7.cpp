static Bigint * s2b (CONST char *s, int nd0, int nd, ULong y9)
{
	Bigint *b;
	int i, k;
	Long x, y;

	x = (nd + 8) / 9;
	for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#ifdef Pack_32
	b = Balloc(k);
	b->x[0] = y9;
	b->wds = 1;
#else
	b = Balloc(k+1);
	b->x[0] = y9 & 0xffff;
	b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

	i = 9;
	if (9 < nd0) {
		s += 9;
		do b = multadd(b, 10, *s++ - '0');
		while(++i < nd0);
		s++;
	} else {
		s += 10;
	}
	for(; i < nd; i++) {
		b = multadd(b, 10, *s++ - '0');
	}
	return b;
}