PHP_FUNCTION(openssl_cipher_iv_length)
{
	char *method;
	int method_len;
	const EVP_CIPHER *cipher_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &method, &method_len) == FAILURE) {
		return;
	}

	if (!method_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	RETURN_LONG(EVP_CIPHER_iv_length(cipher_type));
}