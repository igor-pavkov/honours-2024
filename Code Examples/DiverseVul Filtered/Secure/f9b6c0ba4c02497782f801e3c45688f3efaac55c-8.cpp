void BN_clear_free(BIGNUM *a)
	{
	int i;

	if (a == NULL) return;
	bn_check_top(a);
	if (a->d != NULL)
		{
		OPENSSL_cleanse(a->d,a->dmax*sizeof(a->d[0]));
		if (!(BN_get_flags(a,BN_FLG_STATIC_DATA)))
			OPENSSL_free(a->d);
		}
	i=BN_get_flags(a,BN_FLG_MALLOCED);
	OPENSSL_cleanse(a,sizeof(BIGNUM));
	if (i)
		OPENSSL_free(a);
	}