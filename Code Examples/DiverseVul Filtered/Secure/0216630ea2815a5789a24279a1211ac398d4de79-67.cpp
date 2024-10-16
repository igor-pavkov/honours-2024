static void RSA_get0_factors(const RSA *r, const BIGNUM **p, const BIGNUM **q)
{
	*p = r->p;
	*q = r->q;
}