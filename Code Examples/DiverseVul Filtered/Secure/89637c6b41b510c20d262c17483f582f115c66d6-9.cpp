PHP_FUNCTION(openssl_pbkdf2)
{
	long key_length = 0, iterations = 0;
	char *password; int password_len;
	char *salt; int salt_len;
	char *method; int method_len = 0;
	unsigned char *out_buffer;

	const EVP_MD *digest;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll|s",
				&password, &password_len,
				&salt, &salt_len,
				&key_length, &iterations,
				&method, &method_len) == FAILURE) {
		return;
	}

	if (key_length <= 0 || key_length > INT_MAX) {
		RETURN_FALSE;
	}

	if (method_len) {
		digest = EVP_get_digestbyname(method);
	} else {
		digest = EVP_sha1();
	}

	if (!digest) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	out_buffer = emalloc(key_length + 1);
	out_buffer[key_length] = '\0';

	if (PKCS5_PBKDF2_HMAC(password, password_len, (unsigned char *)salt, salt_len, iterations, digest, key_length, out_buffer) == 1) {
		RETVAL_STRINGL((char *)out_buffer, key_length, 0);
	} else {
		efree(out_buffer);
		RETURN_FALSE;
	}
}