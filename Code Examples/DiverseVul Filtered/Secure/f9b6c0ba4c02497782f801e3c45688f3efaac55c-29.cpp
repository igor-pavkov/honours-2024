int BN_num_bits(const BIGNUM *a)
	{
	int i = a->top - 1;
	bn_check_top(a);

	if (BN_is_zero(a)) return 0;
	return ((i*BN_BITS2) + BN_num_bits_word(a->d[i]));
	}