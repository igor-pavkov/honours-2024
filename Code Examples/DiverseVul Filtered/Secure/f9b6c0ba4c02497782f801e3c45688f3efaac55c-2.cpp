void BN_set_params(int mult, int high, int low, int mont)
	{
	if (mult >= 0)
		{
		if (mult > (int)(sizeof(int)*8)-1)
			mult=sizeof(int)*8-1;
		bn_limit_bits=mult;
		bn_limit_num=1<<mult;
		}
	if (high >= 0)
		{
		if (high > (int)(sizeof(int)*8)-1)
			high=sizeof(int)*8-1;
		bn_limit_bits_high=high;
		bn_limit_num_high=1<<high;
		}
	if (low >= 0)
		{
		if (low > (int)(sizeof(int)*8)-1)
			low=sizeof(int)*8-1;
		bn_limit_bits_low=low;
		bn_limit_num_low=1<<low;
		}
	if (mont >= 0)
		{
		if (mont > (int)(sizeof(int)*8)-1)
			mont=sizeof(int)*8-1;
		bn_limit_bits_mont=mont;
		bn_limit_num_mont=1<<mont;
		}
	}