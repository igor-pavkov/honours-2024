PHP_FUNCTION(openssl_pkcs12_export)
{
	X509 * cert = NULL;
	BIO * bio_out;
	PKCS12 * p12 = NULL;
	zval * zcert = NULL, *zout = NULL, *zpkey, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	zend_resource *keyresource;
	char * pass;
	size_t pass_len;
	char * friendly_name = NULL;
	zval * item;
	STACK_OF(X509) *ca = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz/zs|a", &zcert, &zout, &zpkey, &pass, &pass_len, &args) == FAILURE)
		return;

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get cert from parameter 1");
		return;
	}
	priv_key = php_openssl_evp_from_zval(zpkey, 0, "", 0, 1, &keyresource);
	if (priv_key == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (!X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL, E_WARNING, "private key does not correspond to cert");
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args &&
		(item = zend_hash_str_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name")-1)) != NULL &&
		Z_TYPE_P(item) == IS_STRING
	) {
		friendly_name = Z_STRVAL_P(item);
	}

	if (args && (item = zend_hash_str_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts")-1)) != NULL) {
		ca = php_array_to_X509_sk(item);
	}
	/* end parse extra config */

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);

	if (p12 != NULL) {
		bio_out = BIO_new(BIO_s_mem());
		if (i2d_PKCS12_bio(bio_out, p12)) {
			BUF_MEM *bio_buf;

			zval_dtor(zout);
			BIO_get_mem_ptr(bio_out, &bio_buf);
			ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length);

			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
		}

		BIO_free(bio_out);
		PKCS12_free(p12);
	} else {
		php_openssl_store_errors();
	}
	php_sk_X509_free(ca);

cleanup:

	if (keyresource == NULL && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
}