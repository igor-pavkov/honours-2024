PHP_FUNCTION(openssl_pkcs7_decrypt)
{
	zval * recipcert, * recipkey = NULL;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	zend_resource *certresval, *keyresval;
	BIO * in = NULL, * out = NULL, * datain = NULL;
	PKCS7 * p7 = NULL;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppz|z", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &recipcert, &recipkey) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(recipcert, 0, &certresval);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "unable to coerce parameter 3 to x509 cert");
		goto clean_exit;
	}

	key = php_openssl_evp_from_zval(recipkey ? recipkey : recipcert, 0, "", 0, 0, &keyresval);
	if (key == NULL) {
		php_error_docref(NULL, E_WARNING, "unable to get private key");
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	in = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	out = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (out == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	p7 = SMIME_read_PKCS7(in, &datain);

	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (PKCS7_decrypt(p7, key, cert, out, PKCS7_DETACHED)) {
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
clean_exit:
	PKCS7_free(p7);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(out);
	if (cert && certresval == NULL) {
		X509_free(cert);
	}
	if (key && keyresval == NULL) {
		EVP_PKEY_free(key);
	}
}