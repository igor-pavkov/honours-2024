static void RSA_get0_crt_params(const RSA *r, const BIGNUM **dmp1, const BIGNUM **dmq1, const BIGNUM **iqmp)
{
	*dmp1 = r->dmp1;
	*dmq1 = r->dmq1;
	*iqmp = r->iqmp;
}