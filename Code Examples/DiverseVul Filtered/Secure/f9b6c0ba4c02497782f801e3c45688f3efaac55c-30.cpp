int BN_set_bit(BIGNUM *a, int n)
	{
	int i,j,k;

	if (n < 0)
		return 0;

	i=n/BN_BITS2;
	j=n%BN_BITS2;
	if (a->top <= i)
		{
		if (bn_wexpand(a,i+1) == NULL) return(0);
		for(k=a->top; k<i+1; k++)
			a->d[k]=0;
		a->top=i+1;
		}

	a->d[i]|=(((BN_ULONG)1)<<j);
	bn_check_top(a);
	return(1);
	}