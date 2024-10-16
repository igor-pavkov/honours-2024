PHP_FUNCTION(openssl_x509_fingerprint)
{
	X509 *cert;
	zval *zcert;
	zend_bool raw_output = 0;
	char *method = "sha1";
	size_t method_len;
	zend_string *fingerprint;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|sb", &zcert, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}

	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get cert from parameter 1");
		RETURN_FALSE;
	}

	fingerprint = php_openssl_x509_fingerprint(cert, method, raw_output);
	if (fingerprint) {
		RETVAL_STR(fingerprint);
	} else {
		RETVAL_FALSE;
	}

	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
}