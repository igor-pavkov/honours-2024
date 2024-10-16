PHP_FUNCTION(openssl_csr_export_to_file)
{
	X509_REQ * csr;
	zval * zcsr = NULL;
	zend_bool notext = 1;
	char * filename = NULL;
	size_t filename_len;
	BIO * bio_out;
	zend_resource *csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rp|b", &zcsr, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		return;
	}

	bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (bio_out != NULL) {
		if (!notext && !X509_REQ_print(bio_out, csr)) {
			php_openssl_store_errors();
		}
		if (!PEM_write_bio_X509_REQ(bio_out, csr)) {
			php_error_docref(NULL, E_WARNING, "error writing PEM to file %s", filename);
			php_openssl_store_errors();
		} else {
			RETVAL_TRUE;
		}
		BIO_free(bio_out);
	} else {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error opening file %s", filename);
	}

	if (csr_resource == NULL && csr != NULL) {
		X509_REQ_free(csr);
	}
}