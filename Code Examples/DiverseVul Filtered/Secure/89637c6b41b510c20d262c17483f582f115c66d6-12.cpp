PHP_FUNCTION(openssl_x509_fingerprint)
{
	X509 *cert;
	zval **zcert;
	long certresource;
	zend_bool raw_output = 0;
	char *method = "sha1";
	int method_len;

	char *fingerprint;
	int fingerprint_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|sb", &zcert, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		RETURN_FALSE;
	}

	if (php_openssl_x509_fingerprint(cert, method, raw_output, &fingerprint, &fingerprint_len TSRMLS_CC) == SUCCESS) {
		RETVAL_STRINGL(fingerprint, fingerprint_len, 0);
	} else {
		RETVAL_FALSE;
	}

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
}