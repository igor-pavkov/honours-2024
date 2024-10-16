int DSA_set0_key(DSA *d, BIGNUM *pub_key, BIGNUM *priv_key)
{
	d->pub_key = pub_key;
	d->priv_key = priv_key;

	return 1;
}