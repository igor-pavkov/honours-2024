PHP_FUNCTION(openssl_x509_check_private_key)
{
	zval * zcert, *zkey;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	zend_resource *keyresource = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zcert, &zkey) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		RETURN_FALSE;
	}
	key = php_openssl_evp_from_zval(zkey, 0, "", 0, 1, &keyresource);
	if (key) {
		RETVAL_BOOL(X509_check_private_key(cert, key));
	}

	if (keyresource == NULL && key) {
		EVP_PKEY_free(key);
	}
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
}