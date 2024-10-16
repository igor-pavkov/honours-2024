PHP_FUNCTION(openssl_cipher_iv_length)
{
	char *method;
	size_t method_len;
	const EVP_CIPHER *cipher_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &method, &method_len) == FAILURE) {
		return;
	}

	if (!method_len) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	RETURN_LONG(EVP_CIPHER_iv_length(cipher_type));
}