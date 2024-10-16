PHP_FUNCTION(openssl_x509_checkpurpose)
{
	zval * zcert, * zcainfo = NULL;
	X509_STORE * cainfo = NULL;
	X509 * cert = NULL;
	STACK_OF(X509) * untrustedchain = NULL;
	zend_long purpose;
	char * untrusted = NULL;
	size_t untrusted_len = 0;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl|a!s", &zcert, &purpose, &zcainfo, &untrusted, &untrusted_len) == FAILURE) {
		return;
	}

	RETVAL_LONG(-1);

	if (untrusted) {
		untrustedchain = php_openssl_load_all_certs_from_file(untrusted);
		if (untrustedchain == NULL) {
			goto clean_exit;
		}
	}

	cainfo = php_openssl_setup_verify(zcainfo);
	if (cainfo == NULL) {
		goto clean_exit;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		goto clean_exit;
	}

	ret = check_cert(cainfo, cert, untrustedchain, (int)purpose);
	if (ret != 0 && ret != 1) {
		RETVAL_LONG(ret);
	} else {
		RETVAL_BOOL(ret);
	}
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
clean_exit:
	if (cainfo) {
		X509_STORE_free(cainfo);
	}
	if (untrustedchain) {
		sk_X509_pop_free(untrustedchain, X509_free);
	}
}