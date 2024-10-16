BIGNUM *BN_dup(const BIGNUM *a)
	{
	BIGNUM *t;

	if (a == NULL) return NULL;
	bn_check_top(a);

	t = BN_new();
	if (t == NULL) return NULL;
	if(!BN_copy(t, a))
		{
		BN_free(t);
		return NULL;
		}
	bn_check_top(t);
	return t;
	}