PHP_FUNCTION(openssl_x509_free)
{
	zval *x509;
	X509 *cert;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &x509) == FAILURE) {
		return;
	}
	if ((cert = (X509 *)zend_fetch_resource(Z_RES_P(x509), "OpenSSL X.509", le_x509)) == NULL) {
		RETURN_FALSE;
	}
	zend_list_close(Z_RES_P(x509));
}