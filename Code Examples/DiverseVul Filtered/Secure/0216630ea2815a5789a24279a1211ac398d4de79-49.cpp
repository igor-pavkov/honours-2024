PHP_FUNCTION(openssl_csr_export)
{
	X509_REQ * csr;
	zval * zcsr = NULL, *zout=NULL;
	zend_bool notext = 1;
	BIO * bio_out;
	zend_resource *csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz/|b", &zcsr, &zout, &notext) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	/* export to a var */

	bio_out = BIO_new(BIO_s_mem());
	if (!notext && !X509_REQ_print(bio_out, csr)) {
		php_openssl_store_errors();
	}

	if (PEM_write_bio_X509_REQ(bio_out, csr)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(bio_out, &bio_buf);
		zval_dtor(zout);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length);

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	if (csr_resource == NULL && csr) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}