PHP_FUNCTION(openssl_verify)
{
	zval *key;
	EVP_PKEY *pkey;
	int err = 0;
	EVP_MD_CTX *md_ctx;
	const EVP_MD *mdtype;
	zend_resource *keyresource = NULL;
	char * data;
	size_t data_len;
	char * signature;
	size_t signature_len;
	zval *method = NULL;
	zend_long signature_algo = OPENSSL_ALGO_SHA1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssz|z", &data, &data_len, &signature, &signature_len, &key, &method) == FAILURE) {
		return;
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_UINT(signature_len, signature);

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

	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, 0, &keyresource);
	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "supplied key param cannot be coerced into a public key");
		RETURN_FALSE;
	}

	md_ctx = EVP_MD_CTX_create();
	if (md_ctx == NULL ||
			!EVP_VerifyInit (md_ctx, mdtype) ||
			!EVP_VerifyUpdate (md_ctx, data, data_len) ||
			(err = EVP_VerifyFinal(md_ctx, (unsigned char *)signature, (unsigned int)signature_len, pkey)) < 0) {
		php_openssl_store_errors();
	}
	EVP_MD_CTX_destroy(md_ctx);

	if (keyresource == NULL) {
		EVP_PKEY_free(pkey);
	}
	RETURN_LONG(err);
}