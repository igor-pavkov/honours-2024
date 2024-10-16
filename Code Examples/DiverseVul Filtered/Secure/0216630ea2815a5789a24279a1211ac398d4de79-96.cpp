static int RSA_set0_crt_params(RSA *r, BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp)
{
	r->dmp1 = dmp1;
	r->dmq1 = dmq1;
	r->iqmp = iqmp;

	return 1;
}