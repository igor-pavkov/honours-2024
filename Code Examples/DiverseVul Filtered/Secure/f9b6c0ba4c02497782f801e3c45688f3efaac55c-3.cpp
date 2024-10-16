int bn_cmp_part_words(const BN_ULONG *a, const BN_ULONG *b,
	int cl, int dl)
	{
	int n,i;
	n = cl-1;

	if (dl < 0)
		{
		for (i=dl; i<0; i++)
			{
			if (b[n-i] != 0)
				return -1; /* a < b */
			}
		}
	if (dl > 0)
		{
		for (i=dl; i>0; i--)
			{
			if (a[n+i] != 0)
				return 1; /* a > b */
			}
		}
	return bn_cmp_words(a,b,cl);
	}