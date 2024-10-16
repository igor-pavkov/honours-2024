PHP_FUNCTION(openssl_sign)
{
	zval **key, *signature;
	EVP_PKEY *pkey;
	int siglen;
	unsigned char *sigbuf;
	long keyresource = -1;
	char * data;
	int data_len;
	EVP_MD_CTX md_ctx;
	zval *method = NULL;
	long signature_algo = OPENSSL_ALGO_SHA1;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|z", &data, &data_len, &signature, &key, &method) == FAILURE) {
		return;
	}
	pkey = php_openssl_evp_from_zval(key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a private key");
		RETURN_FALSE;
	}

	if (method == NULL || Z_TYPE_P(method) == IS_LONG) {
		if (method != NULL) {
			signature_algo = Z_LVAL_P(method);
		}
		mdtype = php_openssl_get_evp_md_from_algo(signature_algo);
	} else if (Z_TYPE_P(method) == IS_STRING) {
		mdtype = EVP_get_digestbyname(Z_STRVAL_P(method));
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}
	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}

	siglen = EVP_PKEY_size(pkey);
	sigbuf = emalloc(siglen + 1);

	EVP_SignInit(&md_ctx, mdtype);
	EVP_SignUpdate(&md_ctx, data, data_len);
	if (EVP_SignFinal (&md_ctx, sigbuf,(unsigned int *)&siglen, pkey)) {
		zval_dtor(signature);
		sigbuf[siglen] = '\0';
		ZVAL_STRINGL(signature, (char *)sigbuf, siglen, 0);
		RETVAL_TRUE;
	} else {
		efree(sigbuf);
		RETVAL_FALSE;
	}
	EVP_MD_CTX_cleanup(&md_ctx);
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
}