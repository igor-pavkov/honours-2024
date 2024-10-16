PHP_FUNCTION(openssl_csr_export)
{
	X509_REQ * csr;
	zval * zcsr = NULL, *zout=NULL;
	zend_bool notext = 1;
	BIO * bio_out;

	long csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|b", &zcsr, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(&zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	/* export to a var */

	bio_out = BIO_new(BIO_s_mem());
	if (!notext) {
		X509_REQ_print(bio_out, csr);
	}

	if (PEM_write_bio_X509_REQ(bio_out, csr)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(bio_out, &bio_buf);
		zval_dtor(zout);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}