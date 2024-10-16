PHP_FUNCTION(openssl_open)
{
	zval **privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2;
	unsigned char *buf;
	long keyresource = -1;
	EVP_CIPHER_CTX ctx;
	char * data;	int data_len;
	char * ekey;	int ekey_len;
	char *method =NULL;
	int method_len = 0;
	const EVP_CIPHER *cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szsZ|s", &data, &data_len, &opendata, &ekey, &ekey_len, &privkey, &method, &method_len) == FAILURE) {
		return;
	}

	pkey = php_openssl_evp_from_zval(privkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to coerce parameter 4 into a private key");
		RETURN_FALSE;
	}

	if (method) {
		cipher = EVP_get_cipherbyname(method);
		if (!cipher) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
			RETURN_FALSE;
		}
	} else {
		cipher = EVP_rc4();
	}
	
	buf = emalloc(data_len + 1);

	if (EVP_OpenInit(&ctx, cipher, (unsigned char *)ekey, ekey_len, NULL, pkey) && EVP_OpenUpdate(&ctx, buf, &len1, (unsigned char *)data, data_len)) {
		if (!EVP_OpenFinal(&ctx, buf + len1, &len2) || (len1 + len2 == 0)) {
			efree(buf);
			RETVAL_FALSE;
		} else {
			zval_dtor(opendata);
			buf[len1 + len2] = '\0';
			ZVAL_STRINGL(opendata, erealloc(buf, len1 + len2 + 1), len1 + len2, 0);
			RETVAL_TRUE;
		}
	} else {
		efree(buf);
		RETVAL_FALSE;
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
}