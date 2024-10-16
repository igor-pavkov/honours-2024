PHP_FUNCTION(openssl_x509_read)
{
	zval **cert;
	X509 *x509;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	x509 = php_openssl_x509_from_zval(cert, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (x509 == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied parameter cannot be coerced into an X509 certificate!");
		RETURN_FALSE;
	}
}