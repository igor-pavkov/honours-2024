cdk_error_t cdk_pk_get_fingerprint(cdk_pubkey_t pk, byte * fpr)
{
	digest_hd_st hd;
	int md_algo;
	int dlen = 0;
	int err;
	const mac_entry_st *me;

	if (!pk || !fpr)
		return CDK_Inv_Value;

	if (pk->version < 4 && is_RSA(pk->pubkey_algo))
		md_algo = GNUTLS_DIG_MD5;	/* special */
	else
		md_algo = GNUTLS_DIG_SHA1;

	me = mac_to_entry(md_algo);

	dlen = _gnutls_hash_get_algo_len(me);
	err = _gnutls_hash_init(&hd, me);
	if (err < 0) {
		gnutls_assert();
		return map_gnutls_error(err);
	}
	_cdk_hash_pubkey(pk, &hd, 1);
	_gnutls_hash_deinit(&hd, fpr);
	if (dlen == 16)
		memset(fpr + 16, 0, 4);
	return 0;
}