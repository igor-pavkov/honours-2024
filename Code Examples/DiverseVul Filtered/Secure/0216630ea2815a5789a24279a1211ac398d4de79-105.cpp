PHP_FUNCTION(openssl_x509_export)
{
	X509 * cert;
	zval * zcert, *zout;
	zend_bool notext = 1;
	BIO * bio_out;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz/|b", &zcert, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	bio_out = BIO_new(BIO_s_mem());
	if (!bio_out) {
		php_openssl_store_errors();
		goto cleanup;
	}
	if (!notext && !X509_print(bio_out, cert)) {
		php_openssl_store_errors();
	}
	if (PEM_write_bio_X509(bio_out, cert)) {
		BUF_MEM *bio_buf;

		zval_dtor(zout);
		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length);

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	BIO_free(bio_out);

cleanup:
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
}