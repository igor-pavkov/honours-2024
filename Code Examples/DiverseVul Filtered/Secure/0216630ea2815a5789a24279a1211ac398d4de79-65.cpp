static void DH_get0_pqg(const DH *dh, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g)
{
	*p = dh->p;
	*q = dh->q;
	*g = dh->g;
}