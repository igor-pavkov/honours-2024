void BN_free(BIGNUM *a)
	{
	if (a == NULL) return;
	bn_check_top(a);
	if ((a->d != NULL) && !(BN_get_flags(a,BN_FLG_STATIC_DATA)))
		OPENSSL_free(a->d);
	if (a->flags & BN_FLG_MALLOCED)
		OPENSSL_free(a);
	else
		{
#ifndef OPENSSL_NO_DEPRECATED
		a->flags|=BN_FLG_FREE;
#endif
		a->d = NULL;
		}
	}