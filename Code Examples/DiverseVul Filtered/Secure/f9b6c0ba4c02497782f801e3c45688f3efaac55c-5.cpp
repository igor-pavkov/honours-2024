int ec_GF2m_precompute_mult(EC_GROUP *group, BN_CTX *ctx)
	{
	return ec_wNAF_precompute_mult(group, ctx);
 	}