PHP_FUNCTION(openssl_dh_compute_key)
{
	zval *key;
	char *pub_str;
	size_t pub_len;
	DH *dh;
	EVP_PKEY *pkey;
	BIGNUM *pub;
	zend_string *data;
	int len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sr", &pub_str, &pub_len, &key) == FAILURE) {
		return;
	}
	if ((pkey = (EVP_PKEY *)zend_fetch_resource(Z_RES_P(key), "OpenSSL key", le_key)) == NULL) {
		RETURN_FALSE;
	}
	if (EVP_PKEY_base_id(pkey) != EVP_PKEY_DH) {
		RETURN_FALSE;
	}
	dh = EVP_PKEY_get0_DH(pkey);
	if (dh == NULL) {
		RETURN_FALSE;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(pub_len, pub_key);
	pub = BN_bin2bn((unsigned char*)pub_str, (int)pub_len, NULL);

	data = zend_string_alloc(DH_size(dh), 0);
	len = DH_compute_key((unsigned char*)ZSTR_VAL(data), pub, dh);

	if (len >= 0) {
		ZSTR_LEN(data) = len;
		ZSTR_VAL(data)[len] = 0;
		RETVAL_STR(data);
	} else {
		php_openssl_store_errors();
		zend_string_release(data);
		RETVAL_FALSE;
	}

	BN_free(pub);
}