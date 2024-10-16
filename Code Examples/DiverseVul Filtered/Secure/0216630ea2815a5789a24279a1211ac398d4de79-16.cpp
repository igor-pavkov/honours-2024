PHP_FUNCTION(openssl_pkey_free)
{
	zval *key;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &key) == FAILURE) {
		return;
	}
	if ((pkey = (EVP_PKEY *)zend_fetch_resource(Z_RES_P(key), "OpenSSL key", le_key)) == NULL) {
		RETURN_FALSE;
	}
	zend_list_close(Z_RES_P(key));
}