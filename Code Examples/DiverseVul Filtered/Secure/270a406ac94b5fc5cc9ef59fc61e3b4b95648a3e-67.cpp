PHP_FUNCTION(openssl_x509_free)
{
	zval *x509;
	X509 *cert;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &x509) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(cert, X509 *, &x509, -1, "OpenSSL X.509", le_x509);
	zend_list_delete(Z_LVAL_P(x509));
}