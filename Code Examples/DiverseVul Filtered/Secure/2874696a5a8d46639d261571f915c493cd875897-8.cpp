PHP_FUNCTION(openssl_private_decrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	long padding = RSA_PKCS1_PADDING;
	long keyresource = -1;
	char * data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid private key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_private_decrypt(data_len,
					(unsigned char *)data,
					crypttemp,
					pkey->pkey.rsa,
					padding);
			if (cryptedlen != -1) {
				cryptedbuf = emalloc(cryptedlen + 1);
				memcpy(cryptedbuf, crypttemp, cryptedlen);
				successful = 1;
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
	}

	efree(crypttemp);

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