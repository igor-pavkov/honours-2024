PHP_FUNCTION(openssl_x509_check_private_key)
{
	zval ** zcert, **zkey;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresource = -1, keyresource = -1;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &zcert, &zkey) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		RETURN_FALSE;
	}
	key = php_openssl_evp_from_zval(zkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (key) {
		RETVAL_BOOL(X509_check_private_key(cert, key));
	}

	if (keyresource == -1 && key) {
		EVP_PKEY_free(key);
	}
	if (certresource == -1 && cert) {
		X509_free(cert);
	}
}