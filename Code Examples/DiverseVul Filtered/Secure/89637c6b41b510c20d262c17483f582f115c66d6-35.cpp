PHP_FUNCTION(openssl_spki_verify)
{
	int spkstr_len, i = 0;
	char *spkstr = NULL, * spkstr_cleaned = NULL;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spkstr, &spkstr_len) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (spkstr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied SPKAC");
		goto cleanup;
	}

	spkstr_cleaned = emalloc(spkstr_len + 1);
	openssl_spki_cleanup(spkstr, spkstr_cleaned);

	if (strlen(spkstr_cleaned)<=0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, strlen(spkstr_cleaned));
	if (spki == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	i = NETSCAPE_SPKI_verify(spki, pkey);
	goto cleanup;

cleanup:
	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}

	if (i > 0) {
		RETVAL_TRUE;
	}
}