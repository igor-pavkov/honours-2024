PHP_FUNCTION(openssl_spki_export_challenge)
{
	size_t spkstr_len;
	char *spkstr = NULL, * spkstr_cleaned = NULL;
	int spkstr_cleaned_len;

	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &spkstr, &spkstr_len) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (spkstr == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to use supplied SPKAC");
		goto cleanup;
	}

	spkstr_cleaned = emalloc(spkstr_len + 1);
	spkstr_cleaned_len = (int)(spkstr_len - php_openssl_spki_cleanup(spkstr, spkstr_cleaned));

	if (spkstr_cleaned_len == 0) {
		php_error_docref(NULL, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, spkstr_cleaned_len);
	if (spki == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to decode SPKAC");
		goto cleanup;
	}

	RETVAL_STRING((const char *)ASN1_STRING_get0_data(spki->spkac->challenge));
	goto cleanup;

cleanup:
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
	if (spki) {
		NETSCAPE_SPKI_free(spki);
	}
}