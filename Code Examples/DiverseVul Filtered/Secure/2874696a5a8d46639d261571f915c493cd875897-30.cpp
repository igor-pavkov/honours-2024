PHP_FUNCTION(openssl_pkey_get_private)
{
	zval **cert;
	EVP_PKEY *pkey;
	char * passphrase = "";
	int passphrase_len = sizeof("")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|s", &cert, &passphrase, &passphrase_len) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(cert, 0, passphrase, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
	zend_list_addref(Z_LVAL_P(return_value));
}