void BN_set_negative(BIGNUM *a, int b)
	{
	if (b && !BN_is_zero(a))
		a->neg = 1;
	else
		a->neg = 0;
	}