void bn_mul_normal(BN_ULONG *r, BN_ULONG *a, int na, BN_ULONG *b, int nb)
	{
	BN_ULONG *rr;

#ifdef BN_COUNT
	fprintf(stderr," bn_mul_normal %d * %d\n",na,nb);
#endif

	if (na < nb)
		{
		int itmp;
		BN_ULONG *ltmp;

		itmp=na; na=nb; nb=itmp;
		ltmp=a;   a=b;   b=ltmp;

		}
	rr= &(r[na]);
	if (nb <= 0)
		{
		(void)bn_mul_words(r,a,na,0);
		return;
		}
	else
		rr[0]=bn_mul_words(r,a,na,b[0]);

	for (;;)
		{
		if (--nb <= 0) return;
		rr[1]=bn_mul_add_words(&(r[1]),a,na,b[1]);
		if (--nb <= 0) return;
		rr[2]=bn_mul_add_words(&(r[2]),a,na,b[2]);
		if (--nb <= 0) return;
		rr[3]=bn_mul_add_words(&(r[3]),a,na,b[3]);
		if (--nb <= 0) return;
		rr[4]=bn_mul_add_words(&(r[4]),a,na,b[4]);
		rr+=4;
		r+=4;
		b+=4;
		}
	}