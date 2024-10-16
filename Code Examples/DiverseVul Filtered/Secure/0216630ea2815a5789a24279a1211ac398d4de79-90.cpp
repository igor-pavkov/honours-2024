PHP_FUNCTION(openssl_public_encrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	zend_string *cryptedbuf;
	int successful = 0;
	zend_resource *keyresource = NULL;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/z|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, 0, &keyresource);
	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data);

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = zend_string_alloc(cryptedlen, 0);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt((int)data_len,
						(unsigned char *)data,
						(unsigned char *)ZSTR_VAL(cryptedbuf),
						EVP_PKEY_get0_RSA(pkey),
						(int)padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "key type not supported in this PHP build!");

	}

	if (successful) {
		zval_dtor(crypted);
		ZSTR_VAL(cryptedbuf)[cryptedlen] = '\0';
		ZVAL_NEW_STR(crypted, cryptedbuf);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
	if (keyresource == NULL) {
		EVP_PKEY_free(pkey);
	}
	if (cryptedbuf) {
		zend_string_release(cryptedbuf);
	}
}