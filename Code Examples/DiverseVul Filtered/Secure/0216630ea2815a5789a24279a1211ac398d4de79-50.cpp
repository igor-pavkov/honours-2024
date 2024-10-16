static void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d)
{
	*n = r->n;
	*e = r->e;
	*d = r->d;
}