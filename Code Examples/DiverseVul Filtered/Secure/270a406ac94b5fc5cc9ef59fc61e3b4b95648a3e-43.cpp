PHP_FUNCTION(openssl_csr_get_subject)
{
	zval ** zcsr;
	zend_bool use_shortnames = 1;
	long csr_resource;
	X509_NAME * subject;
	X509_REQ * csr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &zcsr, &use_shortnames) == FAILURE) {
		return;
	}

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);

	if (csr == NULL) {
		RETURN_FALSE;
	}

	subject = X509_REQ_get_subject_name(csr);

	array_init(return_value);
	add_assoc_name_entry(return_value, NULL, subject, use_shortnames TSRMLS_CC);
	return;
}