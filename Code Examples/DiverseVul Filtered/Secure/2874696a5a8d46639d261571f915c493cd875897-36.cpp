PHP_FUNCTION(openssl_verify)
{
	zval **key;
	EVP_PKEY *pkey;
	int err;
	EVP_MD_CTX     md_ctx;
	const EVP_MD *mdtype;
	long keyresource = -1;
	char * data;	int data_len;
	char * signature;	int signature_len;
	zval *method = NULL;
	long signature_algo = OPENSSL_ALGO_SHA1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssZ|z", &data, &data_len, &signature, &signature_len, &key, &method) == FAILURE) {
		return;
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

	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a public key");
		RETURN_FALSE;
	}

	EVP_VerifyInit   (&md_ctx, mdtype);
	EVP_VerifyUpdate (&md_ctx, data, data_len);
	err = EVP_VerifyFinal (&md_ctx, (unsigned char *)signature, signature_len, pkey);
	EVP_MD_CTX_cleanup(&md_ctx);

	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	RETURN_LONG(err);
}