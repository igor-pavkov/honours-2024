int BN_get_params(int which)
	{
	if      (which == 0) return(bn_limit_bits);
	else if (which == 1) return(bn_limit_bits_high);
	else if (which == 2) return(bn_limit_bits_low);
	else if (which == 3) return(bn_limit_bits_mont);
	else return(0);
	}