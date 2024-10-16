PHP_FUNCTION(openssl_csr_sign)
{
	zval ** zcert = NULL, **zcsr, **zpkey, *args = NULL;
	long num_days;
	long serial = 0L;
	X509 * cert = NULL, *new_cert = NULL;
	X509_REQ * csr;
	EVP_PKEY * key = NULL, *priv_key = NULL;
	long csr_resource, certresource = 0, keyresource = -1;
	int i;
	struct php_x509_request req;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ!Zl|a!l", &zcsr, &zcert, &zpkey, &num_days, &args, &serial) == FAILURE)
		return;

	RETVAL_FALSE;
	PHP_SSL_REQ_INIT(&req);

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}
	if (zcert) {
		cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
		if (cert == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 2");
			goto cleanup;
		}
	}
	priv_key = php_openssl_evp_from_zval(zpkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (priv_key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key does not correspond to signing cert");
		goto cleanup;
	}

	if (PHP_SSL_REQ_PARSE(&req, args) == FAILURE) {
		goto cleanup;
	}
	/* Check that the request matches the signature */
	key = X509_REQ_get_pubkey(csr);
	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error unpacking public key");
		goto cleanup;
	}
	i = X509_REQ_verify(csr, key);

	if (i < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Signature verification problems");
		goto cleanup;
	}
	else if (i == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Signature did not match the certificate request");
		goto cleanup;
	}

	/* Now we can get on with it */

	new_cert = X509_new();
	if (new_cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No memory");
		goto cleanup;
	}
	/* Version 3 cert */
	if (!X509_set_version(new_cert, 2))
		goto cleanup;

	ASN1_INTEGER_set(X509_get_serialNumber(new_cert), serial);

	X509_set_subject_name(new_cert, X509_REQ_get_subject_name(csr));

	if (cert == NULL) {
		cert = new_cert;
	}
	if (!X509_set_issuer_name(new_cert, X509_get_subject_name(cert))) {
		goto cleanup;
	}
	X509_gmtime_adj(X509_get_notBefore(new_cert), 0);
	X509_gmtime_adj(X509_get_notAfter(new_cert), (long)60*60*24*num_days);
	i = X509_set_pubkey(new_cert, key);
	if (!i) {
		goto cleanup;
	}
	if (req.extensions_section) {
		X509V3_CTX ctx;

		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_conf_lhash(&ctx, req.req_config);
		if (!X509V3_EXT_add_conf(req.req_config, &ctx, req.extensions_section, new_cert)) {
			goto cleanup;
		}
	}

	/* Now sign it */
	if (!X509_sign(new_cert, priv_key, req.digest)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to sign it");
		goto cleanup;
	}

	/* Succeeded; lets return the cert */
	RETVAL_RESOURCE(zend_list_insert(new_cert, le_x509 TSRMLS_CC));
	new_cert = NULL;

cleanup:

	if (cert == new_cert) {
		cert = NULL;
	}
	PHP_SSL_REQ_DISPOSE(&req);

	if (keyresource == -1 && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (key) {
		EVP_PKEY_free(key);
	}
	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	if (certresource == -1 && cert) {
		X509_free(cert);
	}
	if (new_cert) {
		X509_free(new_cert);
	}
}