PHP_FUNCTION(openssl_x509_read)
{
	zval *cert;
	X509 *x509;
	zend_resource *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE) {
		return;
	}
	x509 = php_openssl_x509_from_zval(cert, 1, &res);
	ZVAL_RES(return_value, res);

	if (x509 == NULL) {
		php_error_docref(NULL, E_WARNING, "supplied parameter cannot be coerced into an X509 certificate!");
		RETURN_FALSE;
	}
}