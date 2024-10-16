PHP_FUNCTION(openssl_digest)
{
	zend_bool raw_output = 0;
	char *data, *method;
	size_t data_len, method_len;
	const EVP_MD *mdtype;
	EVP_MD_CTX *md_ctx;
	unsigned int siglen;
	zend_string *sigbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|b", &data, &data_len, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}
	mdtype = EVP_get_digestbyname(method);
	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	siglen = EVP_MD_size(mdtype);
	sigbuf = zend_string_alloc(siglen, 0);

	md_ctx = EVP_MD_CTX_create();
	if (EVP_DigestInit(md_ctx, mdtype) &&
			EVP_DigestUpdate(md_ctx, (unsigned char *)data, data_len) &&
			EVP_DigestFinal (md_ctx, (unsigned char *)ZSTR_VAL(sigbuf), &siglen)) {
		if (raw_output) {
			ZSTR_VAL(sigbuf)[siglen] = '\0';
			ZSTR_LEN(sigbuf) = siglen;
			RETVAL_STR(sigbuf);
		} else {
			int digest_str_len = siglen * 2;
			zend_string *digest_str = zend_string_alloc(digest_str_len, 0);

			make_digest_ex(ZSTR_VAL(digest_str), (unsigned char*)ZSTR_VAL(sigbuf), siglen);
			ZSTR_VAL(digest_str)[digest_str_len] = '\0';
			zend_string_release(sigbuf);
			RETVAL_STR(digest_str);
		}
	} else {
		php_openssl_store_errors();
		zend_string_release(sigbuf);
		RETVAL_FALSE;
	}

	EVP_MD_CTX_destroy(md_ctx);
}