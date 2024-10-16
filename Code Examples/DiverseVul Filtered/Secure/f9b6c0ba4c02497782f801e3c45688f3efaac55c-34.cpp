void BN_clear(BIGNUM *a)
	{
	bn_check_top(a);
	if (a->d != NULL)
		memset(a->d,0,a->dmax*sizeof(a->d[0]));
	a->top=0;
	a->neg=0;
	}