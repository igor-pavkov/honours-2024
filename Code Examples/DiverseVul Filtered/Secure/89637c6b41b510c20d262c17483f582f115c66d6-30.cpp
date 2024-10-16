PHP_FUNCTION(openssl_pkcs12_export)
{
	X509 * cert = NULL;
	BIO * bio_out;
	PKCS12 * p12 = NULL;
	zval * zcert = NULL, *zout = NULL, *zpkey, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	long certresource, keyresource;
	char * pass;
	int pass_len;
	char * friendly_name = NULL;
	zval ** item;
	STACK_OF(X509) *ca = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzs|a", &zcert, &zout, &zpkey, &pass, &pass_len, &args) == FAILURE)
		return;

	RETVAL_FALSE;
	
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}
	priv_key = php_openssl_evp_from_zval(&zpkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (priv_key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key does not correspond to cert");
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args && zend_hash_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name"), (void**)&item) == SUCCESS && Z_TYPE_PP(item) == IS_STRING)
		friendly_name = Z_STRVAL_PP(item);

	if (args && zend_hash_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts"), (void**)&item) == SUCCESS)
		ca = php_array_to_X509_sk(item TSRMLS_CC);
	/* end parse extra config */
	
	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);

	bio_out = BIO_new(BIO_s_mem());
	if (i2d_PKCS12_bio(bio_out, p12))  {
		BUF_MEM *bio_buf;

		zval_dtor(zout);
		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	BIO_free(bio_out);
	PKCS12_free(p12);
	php_sk_X509_free(ca);
	
cleanup:

	if (keyresource == -1 && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (certresource == -1 && cert) { 
		X509_free(cert);
	}
}