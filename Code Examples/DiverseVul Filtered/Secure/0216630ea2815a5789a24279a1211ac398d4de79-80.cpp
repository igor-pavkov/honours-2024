PHP_FUNCTION(openssl_decrypt)
{
	zend_long options = 0;
	char *data, *method, *password, *iv = "", *tag = NULL, *aad = "";
	size_t data_len, method_len, password_len, iv_len = 0, tag_len = 0, aad_len = 0;
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX *cipher_ctx;
	struct php_openssl_cipher_mode mode;
	int i = 0, outlen;
	zend_string *outbuf;
	zend_string *base64_str = NULL;
	zend_bool free_iv = 0, free_password = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|lsss", &data, &data_len, &method, &method_len,
					&password, &password_len, &options, &iv, &iv_len, &tag, &tag_len, &aad, &aad_len) == FAILURE) {
		return;
	}

	if (!method_len) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(password_len, password);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(aad_len, aad);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(tag_len, tag);

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_ctx = EVP_CIPHER_CTX_new();
	if (!cipher_ctx) {
		php_error_docref(NULL, E_WARNING, "Failed to create cipher context");
		RETURN_FALSE;
	}

	php_openssl_load_cipher_mode(&mode, cipher_type);

	if (!(options & OPENSSL_RAW_DATA)) {
		base64_str = php_base64_decode((unsigned char*)data, data_len);
		if (!base64_str) {
			php_error_docref(NULL, E_WARNING, "Failed to base64 decode the input");
			EVP_CIPHER_CTX_free(cipher_ctx);
			RETURN_FALSE;
		}
		data_len = ZSTR_LEN(base64_str);
		data = ZSTR_VAL(base64_str);
	}

	if (php_openssl_cipher_init(cipher_type, cipher_ctx, &mode,
				&password, &password_len, &free_password,
				&iv, &iv_len, &free_iv, tag, tag_len, options, 0) == FAILURE ||
			php_openssl_cipher_update(cipher_type, cipher_ctx, &mode, &outbuf, &outlen,
				data, data_len, aad, aad_len, 0) == FAILURE) {
		RETVAL_FALSE;
	} else if (mode.is_single_run_aead ||
			EVP_DecryptFinal(cipher_ctx, (unsigned char *)ZSTR_VAL(outbuf) + outlen, &i)) {
		outlen += i;
		ZSTR_VAL(outbuf)[outlen] = '\0';
		ZSTR_LEN(outbuf) = outlen;
		RETVAL_STR(outbuf);
	} else {
		php_openssl_store_errors();
		zend_string_release(outbuf);
		RETVAL_FALSE;
	}

	if (free_password) {
		efree(password);
	}
	if (free_iv) {
		efree(iv);
	}
	if (base64_str) {
		zend_string_release(base64_str);
	}
	EVP_CIPHER_CTX_cleanup(cipher_ctx);
	EVP_CIPHER_CTX_free(cipher_ctx);
}