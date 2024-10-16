PHP_FUNCTION(openssl_x509_checkpurpose)
{
	zval ** zcert, * zcainfo = NULL;
	X509_STORE * cainfo = NULL;
	X509 * cert = NULL;
	long certresource = -1;
	STACK_OF(X509) * untrustedchain = NULL;
	long purpose;
	char * untrusted = NULL;
	int untrusted_len = 0, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl|a!s", &zcert, &purpose, &zcainfo, &untrusted, &untrusted_len) == FAILURE) {
		return;
	}

	RETVAL_LONG(-1);

	if (untrusted) {
		untrustedchain = load_all_certs_from_file(untrusted);
		if (untrustedchain == NULL) {
			goto clean_exit;
		}
	}

	cainfo = setup_verify(zcainfo TSRMLS_CC);
	if (cainfo == NULL) {
		goto clean_exit;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		goto clean_exit;
	}

	ret = check_cert(cainfo, cert, untrustedchain, purpose);
	if (ret != 0 && ret != 1) {
		RETVAL_LONG(ret);
	} else {
		RETVAL_BOOL(ret);
	}

clean_exit:
	if (certresource == 1 && cert) {
		X509_free(cert);
	}
	if (cainfo) {
		X509_STORE_free(cainfo);
	}
	if (untrustedchain) {
		sk_X509_pop_free(untrustedchain, X509_free);
	}
}