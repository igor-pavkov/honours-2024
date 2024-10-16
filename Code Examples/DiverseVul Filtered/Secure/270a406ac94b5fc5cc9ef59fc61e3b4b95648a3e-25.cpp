PHP_FUNCTION(openssl_digest)
{
	zend_bool raw_output = 0;
	char *data, *method;
	int data_len, method_len;
	const EVP_MD *mdtype;
	EVP_MD_CTX md_ctx;
	int siglen;
	unsigned char *sigbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &data, &data_len, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}
	mdtype = EVP_get_digestbyname(method);
	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	siglen = EVP_MD_size(mdtype);
	sigbuf = emalloc(siglen + 1);

	EVP_DigestInit(&md_ctx, mdtype);
	EVP_DigestUpdate(&md_ctx, (unsigned char *)data, data_len);
	if (EVP_DigestFinal (&md_ctx, (unsigned char *)sigbuf, (unsigned int *)&siglen)) {
		if (raw_output) {
			sigbuf[siglen] = '\0';
			RETVAL_STRINGL((char *)sigbuf, siglen, 0);
		} else {
			int digest_str_len = siglen * 2;
			char *digest_str = emalloc(digest_str_len + 1);

			make_digest_ex(digest_str, sigbuf, siglen);
			efree(sigbuf);
			RETVAL_STRINGL(digest_str, digest_str_len, 0);
		}
	} else {
		efree(sigbuf);
		RETVAL_FALSE;
	}
}