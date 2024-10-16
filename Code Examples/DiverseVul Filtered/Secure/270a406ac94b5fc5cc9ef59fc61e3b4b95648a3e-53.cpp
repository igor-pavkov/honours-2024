PHP_FUNCTION(openssl_x509_export)
{
	X509 * cert;
	zval ** zcert, *zout;
	zend_bool notext = 1;
	BIO * bio_out;
	long certresource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zz|b", &zcert, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	bio_out = BIO_new(BIO_s_mem());
	if (!notext) {
		X509_print(bio_out, cert);
	}
	if (PEM_write_bio_X509(bio_out, cert))  {
		BUF_MEM *bio_buf;

		zval_dtor(zout);
		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
	BIO_free(bio_out);
}