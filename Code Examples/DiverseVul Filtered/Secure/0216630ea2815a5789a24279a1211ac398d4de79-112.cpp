PHP_FUNCTION(openssl_pkey_get_private)
{
	zval *cert;
	EVP_PKEY *pkey;
	char * passphrase = "";
	size_t passphrase_len = sizeof("")-1;
	zend_resource *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &cert, &passphrase, &passphrase_len) == FAILURE) {
		return;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase);
	pkey = php_openssl_evp_from_zval(cert, 0, passphrase, passphrase_len, 1, &res);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
	ZVAL_RES(return_value, res);
	Z_ADDREF_P(return_value);
}