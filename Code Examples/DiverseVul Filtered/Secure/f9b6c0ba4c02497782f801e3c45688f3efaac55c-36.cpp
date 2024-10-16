BN_ULONG BN_get_word(const BIGNUM *a)
	{
	if (a->top > 1)
		return BN_MASK2;
	else if (a->top == 1)
		return a->d[0];
	/* a->top == 0 */
	return 0;
	}