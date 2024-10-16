PHP_FUNCTION(openssl_sign)
{
	zval *key, *signature;
	EVP_PKEY *pkey;
	unsigned int siglen;
	zend_string *sigbuf;
	zend_resource *keyresource = NULL;
	char * data;
	size_t data_len;
	EVP_MD_CTX *md_ctx;
	zval *method = NULL;
	zend_long signature_algo = OPENSSL_ALGO_SHA1;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/z|z", &data, &data_len, &signature, &key, &method) == FAILURE) {
		return;
	}
	pkey = php_openssl_evp_from_zval(key, 0, "", 0, 0, &keyresource);
	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "supplied key param cannot be coerced into a private key");
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
		php_error_docref(NULL, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}
	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}

	siglen = EVP_PKEY_size(pkey);
	sigbuf = zend_string_alloc(siglen, 0);

	md_ctx = EVP_MD_CTX_create();
	if (md_ctx != NULL &&
			EVP_SignInit(md_ctx, mdtype) &&
			EVP_SignUpdate(md_ctx, data, data_len) &&
			EVP_SignFinal(md_ctx, (unsigned char*)ZSTR_VAL(sigbuf), &siglen, pkey)) {
		zval_dtor(signature);
		ZSTR_VAL(sigbuf)[siglen] = '\0';
		ZSTR_LEN(sigbuf) = siglen;
		ZVAL_NEW_STR(signature, sigbuf);
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		efree(sigbuf);
		RETVAL_FALSE;
	}
	EVP_MD_CTX_destroy(md_ctx);
	if (keyresource == NULL) {
		EVP_PKEY_free(pkey);
	}
}