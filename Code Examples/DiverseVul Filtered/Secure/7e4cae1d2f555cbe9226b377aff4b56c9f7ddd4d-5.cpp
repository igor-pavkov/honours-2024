void bn_mul_low_normal(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b, int n)
	{
#ifdef BN_COUNT
	fprintf(stderr," bn_mul_low_normal %d * %d\n",n,n);
#endif
	bn_mul_words(r,a,n,b[0]);

	for (;;)
		{
		if (--n <= 0) return;
		bn_mul_add_words(&(r[1]),a,n,b[1]);
		if (--n <= 0) return;
		bn_mul_add_words(&(r[2]),a,n,b[2]);
		if (--n <= 0) return;
		bn_mul_add_words(&(r[3]),a,n,b[3]);
		if (--n <= 0) return;
		bn_mul_add_words(&(r[4]),a,n,b[4]);
		r+=4;
		b+=4;
		}
	}