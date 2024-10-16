PHP_FUNCTION(openssl_x509_export_to_file)
{
	X509 * cert;
	zval * zcert;
	zend_bool notext = 1;
	BIO * bio_out;
	char * filename;
	size_t filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zp|b", &zcert, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		return;
	}

	bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (bio_out) {
		if (!notext && !X509_print(bio_out, cert)) {
			php_openssl_store_errors();
		}
		if (!PEM_write_bio_X509(bio_out, cert)) {
			php_openssl_store_errors();
		}

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error opening file %s", filename);
	}
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}

	if (!BIO_free(bio_out)) {
		php_openssl_store_errors();
	}
}