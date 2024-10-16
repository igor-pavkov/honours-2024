PHP_FUNCTION(openssl_spki_export_challenge)
{
	int spkstr_len;
	char *spkstr = NULL, * spkstr_cleaned = NULL;

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

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, strlen(spkstr_cleaned));
	if (spki == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode SPKAC");
		goto cleanup;
	}

	RETVAL_STRING((char *) ASN1_STRING_data(spki->spkac->challenge), 1);
	goto cleanup;

cleanup:
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
}