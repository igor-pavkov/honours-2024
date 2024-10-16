PHP_FUNCTION(openssl_pkey_free)
{
	zval *key;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	zend_list_delete(Z_LVAL_P(key));
}