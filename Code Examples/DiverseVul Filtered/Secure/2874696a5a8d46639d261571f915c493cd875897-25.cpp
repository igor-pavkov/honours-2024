PHP_FUNCTION(openssl_csr_get_public_key)
{
	zval ** zcsr;
	zend_bool use_shortnames = 1;
	long csr_resource;

	X509_REQ * csr;
	EVP_PKEY *tpubkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &zcsr, &use_shortnames) == FAILURE) {
		return;
	}

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);

	if (csr == NULL) {
		RETURN_FALSE;
	}

	tpubkey=X509_REQ_get_pubkey(csr);
	RETVAL_RESOURCE(zend_list_insert(tpubkey, le_key TSRMLS_CC));
	return;
}