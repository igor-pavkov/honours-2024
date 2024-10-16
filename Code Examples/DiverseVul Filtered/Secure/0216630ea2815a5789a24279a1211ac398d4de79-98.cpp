PHP_FUNCTION(openssl_spki_new)
{
	size_t challenge_len;
	char * challenge = NULL, * spkstr = NULL;
	zend_string * s = NULL;
	zend_resource *keyresource = NULL;
	const char *spkac = "SPKAC=";
	zend_long algo = OPENSSL_ALGO_MD5;

	zval *method = NULL;
	zval * zpkey = NULL;
	EVP_PKEY * pkey = NULL;
	NETSCAPE_SPKI *spki=NULL;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|z", &zpkey, &challenge, &challenge_len, &method) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(challenge_len, challenge);
	pkey = php_openssl_evp_from_zval(zpkey, 0, challenge, challenge_len, 1, &keyresource);

	if (pkey == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to use supplied private key");
		goto cleanup;
	}

	if (method != NULL) {
		if (Z_TYPE_P(method) == IS_LONG) {
			algo = Z_LVAL_P(method);
		} else {
			php_error_docref(NULL, E_WARNING, "Algorithm must be of supported type");
			goto cleanup;
		}
	}
	mdtype = php_openssl_get_evp_md_from_algo(algo);

	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown signature algorithm");
		goto cleanup;
	}

	if ((spki = NETSCAPE_SPKI_new()) == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to create new SPKAC");
		goto cleanup;
	}

	if (challenge) {
		if (!ASN1_STRING_set(spki->spkac->challenge, challenge, (int)challenge_len)) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Unable to set challenge data");
			goto cleanup;
		}
	}

	if (!NETSCAPE_SPKI_set_pubkey(spki, pkey)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to embed public key");
		goto cleanup;
	}

	if (!NETSCAPE_SPKI_sign(spki, pkey, mdtype)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to sign with specified algorithm");
		goto cleanup;
	}

	spkstr = NETSCAPE_SPKI_b64_encode(spki);
	if (!spkstr){
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to encode SPKAC");
		goto cleanup;
	}

	s = zend_string_alloc(strlen(spkac) + strlen(spkstr), 0);
	sprintf(ZSTR_VAL(s), "%s%s", spkac, spkstr);
	ZSTR_LEN(s) = strlen(ZSTR_VAL(s));
	OPENSSL_free(spkstr);

	RETVAL_STR(s);
	goto cleanup;

cleanup:

	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (keyresource == NULL && pkey != NULL) {
		EVP_PKEY_free(pkey);
	}

	if (s && ZSTR_LEN(s) <= 0) {
		RETVAL_FALSE;
	}

	if (keyresource == NULL && s != NULL) {
		zend_string_release(s);
	}
}