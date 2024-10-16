PHP_FUNCTION(openssl_pkcs7_sign)
{
	zval * zcert, * zprivkey, * zheaders;
	zval * hval;
	X509 * cert = NULL;
	EVP_PKEY * privkey = NULL;
	zend_long flags = PKCS7_DETACHED;
	PKCS7 * p7 = NULL;
	BIO * infile = NULL, * outfile = NULL;
	STACK_OF(X509) *others = NULL;
	zend_resource *certresource = NULL, *keyresource = NULL;
	zend_string * strindex;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;
	char * extracertsfilename = NULL;
	size_t extracertsfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppzza!|lp!",
				&infilename, &infilename_len, &outfilename, &outfilename_len,
				&zcert, &zprivkey, &zheaders, &flags, &extracertsfilename,
				&extracertsfilename_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	if (extracertsfilename) {
		others = php_openssl_load_all_certs_from_file(extracertsfilename);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	privkey = php_openssl_evp_from_zval(zprivkey, 0, "", 0, 0, &keyresource);
	if (privkey == NULL) {
		php_error_docref(NULL, E_WARNING, "error getting private key");
		goto clean_exit;
	}

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "error getting cert");
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (outfile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error opening output file %s!", outfilename);
		goto clean_exit;
	}

	p7 = PKCS7_sign(cert, privkey, others, infile, (int)flags);
	if (p7 == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "error creating PKCS7 structure!");
		goto clean_exit;
	}

	(void)BIO_reset(infile);

	/* tack on extra headers */
	if (zheaders) {
		int ret;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, hval) {
			convert_to_string_ex(hval);

			if (strindex) {
				ret = BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), Z_STRVAL_P(hval));
			} else {
				ret = BIO_printf(outfile, "%s\n", Z_STRVAL_P(hval));
			}
			if (ret < 0) {
				php_openssl_store_errors();
			}
		} ZEND_HASH_FOREACH_END();
	}
	/* write the signed data */
	if (!SMIME_write_PKCS7(outfile, p7, infile, (int)flags)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (others) {
		sk_X509_pop_free(others, X509_free);
	}
	if (privkey && keyresource == NULL) {
		EVP_PKEY_free(privkey);
	}
	if (cert && certresource == NULL) {
		X509_free(cert);
	}
}