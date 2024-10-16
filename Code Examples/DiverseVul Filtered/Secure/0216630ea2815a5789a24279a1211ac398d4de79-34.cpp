PHP_FUNCTION(openssl_encrypt)
{
	zend_long options = 0, tag_len = 16;
	char *data, *method, *password, *iv = "", *aad = "";
	size_t data_len, method_len, password_len, iv_len = 0, aad_len = 0;
	zval *tag = NULL;
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX *cipher_ctx;
	struct php_openssl_cipher_mode mode;
	int i=0, outlen;
	zend_string *outbuf;
	zend_bool free_iv = 0, free_password = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|lsz/sl", &data, &data_len, &method, &method_len,
					&password, &password_len, &options, &iv, &iv_len, &tag, &aad, &aad_len, &tag_len) == FAILURE) {
		return;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(password_len, password);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(aad_len, aad);
	PHP_OPENSSL_CHECK_LONG_TO_INT(tag_len, tag_len);

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

	if (php_openssl_cipher_init(cipher_type, cipher_ctx, &mode,
				&password, &password_len, &free_password,
				&iv, &iv_len, &free_iv, NULL, tag_len, options, 1) == FAILURE ||
			php_openssl_cipher_update(cipher_type, cipher_ctx, &mode, &outbuf, &outlen,
				data, data_len, aad, aad_len, 1) == FAILURE) {
		RETVAL_FALSE;
	} else if (EVP_EncryptFinal(cipher_ctx, (unsigned char *)ZSTR_VAL(outbuf) + outlen, &i)) {
		outlen += i;
		if (options & OPENSSL_RAW_DATA) {
			ZSTR_VAL(outbuf)[outlen] = '\0';
			ZSTR_LEN(outbuf) = outlen;
			RETVAL_STR(outbuf);
		} else {
			zend_string *base64_str;

			base64_str = php_base64_encode((unsigned char*)ZSTR_VAL(outbuf), outlen);
			zend_string_release(outbuf);
			outbuf = base64_str;
			RETVAL_STR(base64_str);
		}
		if (mode.is_aead && tag) {
			zend_string *tag_str = zend_string_alloc(tag_len, 0);

			if (EVP_CIPHER_CTX_ctrl(cipher_ctx, mode.aead_get_tag_flag, tag_len, ZSTR_VAL(tag_str)) == 1) {
				zval_dtor(tag);
				ZSTR_VAL(tag_str)[tag_len] = '\0';
				ZSTR_LEN(tag_str) = tag_len;
				ZVAL_NEW_STR(tag, tag_str);
			} else {
				php_error_docref(NULL, E_WARNING, "Retrieving verification tag failed");
				zend_string_release(tag_str);
				zend_string_release(outbuf);
				RETVAL_FALSE;
			}
		} else if (tag) {
			zval_dtor(tag);
			ZVAL_NULL(tag);
			php_error_docref(NULL, E_WARNING,
					"The authenticated tag cannot be provided for cipher that doesn not support AEAD");
		} else if (mode.is_aead) {
			php_error_docref(NULL, E_WARNING, "A tag should be provided when using AEAD mode");
			zend_string_release(outbuf);
			RETVAL_FALSE;
		}
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
	EVP_CIPHER_CTX_cleanup(cipher_ctx);
	EVP_CIPHER_CTX_free(cipher_ctx);
}