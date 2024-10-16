PHP_FUNCTION(openssl_pkey_get_public)
{
	zval *cert;
	EVP_PKEY *pkey;
	zend_resource *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE) {
		return;
	}
	pkey = php_openssl_evp_from_zval(cert, 1, NULL, 0, 1, &res);
	if (pkey == NULL) {
		RETURN_FALSE;
	}
	ZVAL_RES(return_value, res);
	Z_ADDREF_P(return_value);
}