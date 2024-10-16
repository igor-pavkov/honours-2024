PHP_FUNCTION(openssl_open)
{
	zval *privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2, cipher_iv_len;
	unsigned char *buf, *iv_buf;
	zend_resource *keyresource = NULL;
	EVP_CIPHER_CTX *ctx;
	char * data;
	size_t data_len;
	char * ekey;
	size_t ekey_len;
	char *method = NULL, *iv = NULL;
	size_t method_len = 0, iv_len = 0;
	const EVP_CIPHER *cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/sz|ss", &data, &data_len, &opendata,
				&ekey, &ekey_len, &privkey, &method, &method_len, &iv, &iv_len) == FAILURE) {
		return;
	}

	pkey = php_openssl_evp_from_zval(privkey, 0, "", 0, 0, &keyresource);
	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "unable to coerce parameter 4 into a private key");
		RETURN_FALSE;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(ekey_len, ekey);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data);

	if (method) {
		cipher = EVP_get_cipherbyname(method);
		if (!cipher) {
			php_error_docref(NULL, E_WARNING, "Unknown signature algorithm.");
			RETURN_FALSE;
		}
	} else {
		cipher = EVP_rc4();
	}

	cipher_iv_len = EVP_CIPHER_iv_length(cipher);
	if (cipher_iv_len > 0) {
		if (!iv) {
			php_error_docref(NULL, E_WARNING,
					"Cipher algorithm requires an IV to be supplied as a sixth parameter");
			RETURN_FALSE;
		}
		if ((size_t)cipher_iv_len != iv_len) {
			php_error_docref(NULL, E_WARNING, "IV length is invalid");
			RETURN_FALSE;
		}
		iv_buf = (unsigned char *)iv;
	} else {
		iv_buf = NULL;
	}

	buf = emalloc(data_len + 1);

	ctx = EVP_CIPHER_CTX_new();
	if (ctx != NULL && EVP_OpenInit(ctx, cipher, (unsigned char *)ekey, (int)ekey_len, iv_buf, pkey) &&
			EVP_OpenUpdate(ctx, buf, &len1, (unsigned char *)data, (int)data_len) &&
			EVP_OpenFinal(ctx, buf + len1, &len2) && (len1 + len2 > 0)) {
		zval_dtor(opendata);
		buf[len1 + len2] = '\0';
		ZVAL_NEW_STR(opendata, zend_string_init((char*)buf, len1 + len2, 0));
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		RETVAL_FALSE;
	}

	efree(buf);
	if (keyresource == NULL) {
		EVP_PKEY_free(pkey);
	}
	EVP_CIPHER_CTX_free(ctx);
}