static BIGNUM *php_openssl_dh_pub_from_priv(BIGNUM *priv_key, BIGNUM *g, BIGNUM *p)
{
	BIGNUM *pub_key, *priv_key_const_time;
	BN_CTX *ctx;

	pub_key = BN_new();
	if (pub_key == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	priv_key_const_time = BN_new();
	if (priv_key_const_time == NULL) {
		BN_free(pub_key);
		php_openssl_store_errors();
		return NULL;
	}
	ctx = BN_CTX_new();
	if (ctx == NULL) {
		BN_free(pub_key);
		BN_free(priv_key_const_time);
		php_openssl_store_errors();
		return NULL;
	}

	BN_with_flags(priv_key_const_time, priv_key, BN_FLG_CONSTTIME);

	if (!BN_mod_exp_mont(pub_key, g, priv_key_const_time, p, ctx, NULL)) {
		BN_free(pub_key);
		php_openssl_store_errors();
		pub_key = NULL;
	}

	BN_free(priv_key_const_time);
	BN_CTX_free(ctx);

	return pub_key;
}