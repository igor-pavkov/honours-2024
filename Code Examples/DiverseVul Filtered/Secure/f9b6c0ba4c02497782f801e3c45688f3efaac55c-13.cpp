void BN_init(BIGNUM *a)
	{
	memset(a,0,sizeof(BIGNUM));
	bn_check_top(a);
	}