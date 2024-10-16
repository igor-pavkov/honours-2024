int BN_is_bit_set(const BIGNUM *a, int n)
	{
	int i,j;

	bn_check_top(a);
	if (n < 0) return 0;
	i=n/BN_BITS2;
	j=n%BN_BITS2;
	if (a->top <= i) return 0;
	return (int)(((a->d[i])>>j)&((BN_ULONG)1));
	}