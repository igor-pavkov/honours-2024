PHP_FUNCTION(openssl_csr_sign)
{
	zval * zcert = NULL, *zcsr, *zpkey, *args = NULL;
	zend_long num_days;
	zend_long serial = Z_L(0);
	X509 * cert = NULL, *new_cert = NULL;
	X509_REQ * csr;
	EVP_PKEY * key = NULL, *priv_key = NULL;
	zend_resource *csr_resource, *certresource = NULL, *keyresource = NULL;
	int i;
	struct php_x509_request req;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz!zl|a!l", &zcsr, &zcert, &zpkey, &num_days, &args, &serial) == FAILURE)
		return;

	RETVAL_FALSE;
	PHP_SSL_REQ_INIT(&req);

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}
	if (zcert) {
		cert = php_openssl_x509_from_zval(zcert, 0, &certresource);
		if (cert == NULL) {
			php_error_docref(NULL, E_WARNING, "cannot get cert from parameter 2");
			goto cleanup;
		}
	}
	priv_key = php_openssl_evp_from_zval(zpkey, 0, "", 0, 1, &keyresource);
	if (priv_key == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "private key does not correspond to signing cert");
		goto cleanup;
	}

	if (PHP_SSL_REQ_PARSE(&req, args) == FAILURE) {
		goto cleanup;
	}
	/* Check that the request matches the signature */
	key = X509_REQ_get_pubkey(csr);
	if (key == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error unpacking public key");
		goto cleanup;
	}
	i = X509_REQ_verify(csr, key);

	if (i < 0) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Signature verification problems");
		goto cleanup;
	}
	else if (i == 0) {
		php_error_docref(NULL, E_WARNING, "Signature did not match the certificate request");
		goto cleanup;
	}

	/* Now we can get on with it */

	new_cert = X509_new();
	if (new_cert == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "No memory");
		goto cleanup;
	}
	/* Version 3 cert */
	if (!X509_set_version(new_cert, 2)) {
		goto cleanup;
	}


	ASN1_INTEGER_set(X509_get_serialNumber(new_cert), (long)serial);

	X509_set_subject_name(new_cert, X509_REQ_get_subject_name(csr));

	if (cert == NULL) {
		cert = new_cert;
	}
	if (!X509_set_issuer_name(new_cert, X509_get_subject_name(cert))) {
		php_openssl_store_errors();
		goto cleanup;
	}
	X509_gmtime_adj(X509_get_notBefore(new_cert), 0);
	X509_gmtime_adj(X509_get_notAfter(new_cert), 60*60*24*(long)num_days);
	i = X509_set_pubkey(new_cert, key);
	if (!i) {
		php_openssl_store_errors();
		goto cleanup;
	}
	if (req.extensions_section) {
		X509V3_CTX ctx;

		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_conf_lhash(&ctx, req.req_config);
		if (!X509V3_EXT_add_conf(req.req_config, &ctx, req.extensions_section, new_cert)) {
			php_openssl_store_errors();
			goto cleanup;
		}
	}

	/* Now sign it */
	if (!X509_sign(new_cert, priv_key, req.digest)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "failed to sign it");
		goto cleanup;
	}

	/* Succeeded; lets return the cert */
	ZVAL_RES(return_value, zend_register_resource(new_cert, le_x509));
	new_cert = NULL;

cleanup:

	if (cert == new_cert) {
		cert = NULL;
	}
	PHP_SSL_REQ_DISPOSE(&req);

	if (keyresource == NULL && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (key) {
		EVP_PKEY_free(key);
	}
	if (csr_resource == NULL && csr) {
		X509_REQ_free(csr);
	}
	if (zcert && certresource == NULL && cert) {
		X509_free(cert);
	}
	if (new_cert) {
		X509_free(new_cert);
	}
}