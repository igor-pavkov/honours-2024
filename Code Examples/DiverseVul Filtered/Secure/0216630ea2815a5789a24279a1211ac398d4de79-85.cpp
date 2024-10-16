PHP_FUNCTION(openssl_spki_export)
{
	size_t spkstr_len;
	char *spkstr = NULL, * spkstr_cleaned = NULL, * s = NULL;
	int spkstr_cleaned_len;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;
	BIO *out = NULL;

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
		php_error_docref(NULL, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	out = BIO_new(BIO_s_mem());
	if (out && PEM_write_bio_PUBKEY(out, pkey)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(out, &bio_buf);
		RETVAL_STRINGL((char *)bio_buf->data, bio_buf->length);
	} else {
		php_openssl_store_errors();
	}
	goto cleanup;

cleanup:

	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (out != NULL) {
		BIO_free_all(out);
	}
	if (pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
	if (s != NULL) {
		efree(s);
	}
}