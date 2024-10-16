PHP_FUNCTION(openssl_public_encrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt(data_len,
						(unsigned char *)data,
						cryptedbuf,
						pkey->pkey.rsa,
						padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");

	}

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, (char *)cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	if (cryptedbuf) {
		efree(cryptedbuf);
	}
}