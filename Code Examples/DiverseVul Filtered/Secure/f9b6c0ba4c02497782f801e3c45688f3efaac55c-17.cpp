int ec_GF2m_have_precompute_mult(const EC_GROUP *group)
	{
	return ec_wNAF_have_precompute_mult(group);
 	}