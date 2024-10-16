int php_openssl_x509_fingerprint(X509 *peer, const char *method, zend_bool raw, char **out, int *out_len TSRMLS_DC)
{
	unsigned char md[EVP_MAX_MD_SIZE];
	const EVP_MD *mdtype;
	unsigned int n;

	if (!(mdtype = EVP_get_digestbyname(method))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		return FAILURE;
	} else if (!X509_digest(peer, mdtype, md, &n)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not generate signature");
		return FAILURE;
	}

	if (raw) {
		*out_len = n;
		*out = estrndup((char *) md, n);
	} else {
		*out_len = n * 2;
		*out = emalloc(*out_len + 1);

		make_digest_ex(*out, md, n);
	}

	return SUCCESS;
}