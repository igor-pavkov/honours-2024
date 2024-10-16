PHP_FUNCTION(openssl_pkey_get_public)
{
	zval **cert;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(cert, 1, NULL, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
	zend_list_addref(Z_LVAL_P(return_value));
}