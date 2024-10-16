int bn_cmp_words(const BN_ULONG *a, const BN_ULONG *b, int n)
	{
	int i;
	BN_ULONG aa,bb;

	aa=a[n-1];
	bb=b[n-1];
	if (aa != bb) return((aa > bb)?1:-1);
	for (i=n-2; i>=0; i--)
		{
		aa=a[i];
		bb=b[i];
		if (aa != bb) return((aa > bb)?1:-1);
		}
	return(0);
	}