PHP_FUNCTION(openssl_pkcs12_export_to_file)
{
	X509 * cert = NULL;
	BIO * bio_out = NULL;
	PKCS12 * p12 = NULL;
	char * filename;
	char * friendly_name = NULL;
	size_t filename_len;
	char * pass;
	size_t pass_len;
	zval *zcert = NULL, *zpkey = NULL, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	zend_resource *keyresource;
	zval * item;
	STACK_OF(X509) *ca = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zpzs|a", &zcert, &filename, &filename_len, &zpkey, &pass, &pass_len, &args) == FAILURE)
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
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "private key does not correspond to cert");
		goto cleanup;
	}
	if (php_openssl_open_base_dir_chk(filename)) {
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args &&
		(item = zend_hash_str_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name")-1)) != NULL &&
		Z_TYPE_P(item) == IS_STRING
	) {
		friendly_name = Z_STRVAL_P(item);
	}
	/* certpbe (default RC2-40)
	   keypbe (default 3DES)
	   friendly_caname
	*/

	if (args && (item = zend_hash_str_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts")-1)) != NULL) {
		ca = php_array_to_X509_sk(item);
	}
	/* end parse extra config */

	/*PKCS12 *PKCS12_create(char *pass, char *name, EVP_PKEY *pkey, X509 *cert, STACK_OF(X509) *ca,
				int nid_key, int nid_cert, int iter, int mac_iter, int keytype);*/

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);
	if (p12 != NULL) {
		bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (bio_out != NULL) {

			i2d_PKCS12_bio(bio_out, p12);
			BIO_free(bio_out);

			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "error opening file %s", filename);
		}

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