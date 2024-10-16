int BN_mask_bits(BIGNUM *a, int n)
	{
	int b,w;

	bn_check_top(a);
	if (n < 0) return 0;

	w=n/BN_BITS2;
	b=n%BN_BITS2;
	if (w >= a->top) return 0;
	if (b == 0)
		a->top=w;
	else
		{
		a->top=w+1;
		a->d[w]&= ~(BN_MASK2<<b);
		}
	bn_correct_top(a);
	return(1);
	}