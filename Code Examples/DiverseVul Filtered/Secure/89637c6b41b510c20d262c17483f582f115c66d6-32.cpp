PHP_FUNCTION(openssl_spki_new)
{
	int challenge_len;
	char * challenge = NULL, * spkstr = NULL, * s = NULL;
	long keyresource = -1;
	const char *spkac = "SPKAC=";
	long algo = OPENSSL_ALGO_MD5;

	zval *method = NULL;
	zval * zpkey = NULL;
	EVP_PKEY * pkey = NULL;
	NETSCAPE_SPKI *spki=NULL;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zpkey, &challenge, &challenge_len, &method) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(&zpkey, 0, challenge, 1, &keyresource TSRMLS_CC);

	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied private key");
		goto cleanup;
	}

	if (method != NULL) {
		if (Z_TYPE_P(method) == IS_LONG) {
			algo = Z_LVAL_P(method);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Algorithm must be of supported type");
			goto cleanup;
		}
	}
	mdtype = php_openssl_get_evp_md_from_algo(algo);

	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		goto cleanup;
	}

	if ((spki = NETSCAPE_SPKI_new()) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create new SPKAC");
		goto cleanup;
	}

	if (challenge) {
		ASN1_STRING_set(spki->spkac->challenge, challenge, challenge_len);
	}

	if (!NETSCAPE_SPKI_set_pubkey(spki, pkey)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to embed public key");
		goto cleanup;
	}

	if (!NETSCAPE_SPKI_sign(spki, pkey, mdtype)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to sign with specified algorithm");
		goto cleanup;
	}

	spkstr = NETSCAPE_SPKI_b64_encode(spki);
	if (!spkstr){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to encode SPKAC");
		goto cleanup;
	}

	s = emalloc(strlen(spkac) + strlen(spkstr) + 1);
	sprintf(s, "%s%s", spkac, spkstr);

	RETVAL_STRINGL(s, strlen(s), 0);
	goto cleanup;

cleanup:

	if (keyresource == -1 && spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (keyresource == -1 && pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (keyresource == -1 && spkstr != NULL) {
		efree(spkstr);
	}

	if (s && strlen(s) <= 0) {
		RETVAL_FALSE;
	}

	if (keyresource == -1 && s != NULL) {
		efree(s);
	}
}