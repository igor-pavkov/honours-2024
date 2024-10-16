BIGNUM *bn_dup_expand(const BIGNUM *b, int words)
	{
	BIGNUM *r = NULL;

	bn_check_top(b);

	/* This function does not work if
	 *      words <= b->dmax && top < words
	 * because BN_dup() does not preserve 'dmax'!
	 * (But bn_dup_expand() is not used anywhere yet.)
	 */

	if (words > b->dmax)
		{
		BN_ULONG *a = bn_expand_internal(b, words);

		if (a)
			{
			r = BN_new();
			if (r)
				{
				r->top = b->top;
				r->dmax = words;
				r->neg = b->neg;
				r->d = a;
				}
			else
				{
				/* r == NULL, BN_new failure */
				OPENSSL_free(a);
				}
			}
		/* If a == NULL, there was an error in allocation in
		   bn_expand_internal(), and NULL should be returned */
		}
	else
		{
		r = BN_dup(b);
		}

	bn_check_top(r);
	return r;
	}