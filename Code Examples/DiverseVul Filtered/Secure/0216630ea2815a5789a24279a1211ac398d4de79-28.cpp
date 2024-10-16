PHP_FUNCTION(openssl_pbkdf2)
{
	zend_long key_length = 0, iterations = 0;
	char *password;
	size_t password_len;
	char *salt;
	size_t salt_len;
	char *method;
	size_t method_len = 0;
	zend_string *out_buffer;

	const EVP_MD *digest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssll|s",
				&password, &password_len,
				&salt, &salt_len,
				&key_length, &iterations,
				&method, &method_len) == FAILURE) {
		return;
	}

	if (key_length <= 0) {
		RETURN_FALSE;
	}

	if (method_len) {
		digest = EVP_get_digestbyname(method);
	} else {
		digest = EVP_sha1();
	}

	if (!digest) {
		php_error_docref(NULL, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	PHP_OPENSSL_CHECK_LONG_TO_INT(key_length, key);
	PHP_OPENSSL_CHECK_LONG_TO_INT(iterations, iterations);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(password_len, password);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(salt_len, salt);

	out_buffer = zend_string_alloc(key_length, 0);

	if (PKCS5_PBKDF2_HMAC(password, (int)password_len, (unsigned char *)salt, (int)salt_len, (int)iterations, digest, (int)key_length, (unsigned char*)ZSTR_VAL(out_buffer)) == 1) {
		ZSTR_VAL(out_buffer)[key_length] = 0;
		RETURN_NEW_STR(out_buffer);
	} else {
		php_openssl_store_errors();
		zend_string_release(out_buffer);
		RETURN_FALSE;
	}
}