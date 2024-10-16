PHP_FUNCTION(openssl_dh_compute_key)
{
	zval *key;
	char *pub_str;
	int pub_len;
	EVP_PKEY *pkey;
	BIGNUM *pub;
	char *data;
	int len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &pub_str, &pub_len, &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	if (!pkey || EVP_PKEY_type(pkey->type) != EVP_PKEY_DH || !pkey->pkey.dh) {
		RETURN_FALSE;
	}

	pub = BN_bin2bn((unsigned char*)pub_str, pub_len, NULL);

	data = emalloc(DH_size(pkey->pkey.dh) + 1);
	len = DH_compute_key((unsigned char*)data, pub, pkey->pkey.dh);

	if (len >= 0) {
		data[len] = 0;
		RETVAL_STRINGL(data, len, 0);
	} else {
		efree(data);
		RETVAL_FALSE;
	}

	BN_free(pub);
}