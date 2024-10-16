int BN_clear_bit(BIGNUM *a, int n)
	{
	int i,j;

	bn_check_top(a);
	if (n < 0) return 0;

	i=n/BN_BITS2;
	j=n%BN_BITS2;
	if (a->top <= i) return(0);

	a->d[i]&=(~(((BN_ULONG)1)<<j));
	bn_correct_top(a);
	return(1);
	}