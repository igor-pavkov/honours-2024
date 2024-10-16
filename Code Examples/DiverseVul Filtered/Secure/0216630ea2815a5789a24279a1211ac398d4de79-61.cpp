PHP_FUNCTION(openssl_pkcs7_verify)
{
	X509_STORE * store = NULL;
	zval * cainfo = NULL;
	STACK_OF(X509) *signers= NULL;
	STACK_OF(X509) *others = NULL;
	PKCS7 * p7 = NULL;
	BIO * in = NULL, * datain = NULL, * dataout = NULL, * p7bout  = NULL;
	zend_long flags = 0;
	char * filename;
	size_t filename_len;
	char * extracerts = NULL;
	size_t extracerts_len = 0;
	char * signersfilename = NULL;
	size_t signersfilename_len = 0;
	char * datafilename = NULL;
	size_t datafilename_len = 0;
	char * p7bfilename = NULL;
	size_t p7bfilename_len = 0;

	RETVAL_LONG(-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "pl|pappp", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len, &datafilename, &datafilename_len, &p7bfilename, &p7bfilename_len) == FAILURE) {
		return;
	}

	if (extracerts) {
		others = php_openssl_load_all_certs_from_file(extracerts);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	flags = flags & ~PKCS7_DETACHED;

	store = php_openssl_setup_verify(cainfo);

	if (!store) {
		goto clean_exit;
	}
	if (php_openssl_open_base_dir_chk(filename)) {
		goto clean_exit;
	}

	in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	p7 = SMIME_read_PKCS7(in, &datain);
	if (p7 == NULL) {
#if DEBUG_SMIME
		zend_printf("SMIME_read_PKCS7 failed\n");
#endif
		php_openssl_store_errors();
		goto clean_exit;
	}

	if (datafilename) {

		if (php_openssl_open_base_dir_chk(datafilename)) {
			goto clean_exit;
		}

		dataout = BIO_new_file(datafilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (dataout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}

	if (p7bfilename) {

		if (php_openssl_open_base_dir_chk(p7bfilename)) {
			goto clean_exit;
		}

		p7bout = BIO_new_file(p7bfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (p7bout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}
#if DEBUG_SMIME
	zend_printf("Calling PKCS7 verify\n");
#endif

	if (PKCS7_verify(p7, others, store, datain, dataout, (int)flags)) {

		RETVAL_TRUE;

		if (signersfilename) {
			BIO *certout;

			if (php_openssl_open_base_dir_chk(signersfilename)) {
				goto clean_exit;
			}

			certout = BIO_new_file(signersfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
			if (certout) {
				int i;
				signers = PKCS7_get0_signers(p7, NULL, (int)flags);
				if (signers != NULL) {

					for (i = 0; i < sk_X509_num(signers); i++) {
						if (!PEM_write_bio_X509(certout, sk_X509_value(signers, i))) {
							php_openssl_store_errors();
							RETVAL_LONG(-1);
							php_error_docref(NULL, E_WARNING, "failed to write signer %d", i);
						}
					}

					sk_X509_free(signers);
				} else {
					RETVAL_LONG(-1);
					php_openssl_store_errors();
				}

				BIO_free(certout);
			} else {
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_LONG(-1);
			}

			if (p7bout) {
				PEM_write_bio_PKCS7(p7bout, p7);
			}
		}
	} else {
		php_openssl_store_errors();
		RETVAL_FALSE;
	}
clean_exit:
	if (p7bout) {
		BIO_free(p7bout);
	}
	X509_STORE_free(store);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(dataout);
	PKCS7_free(p7);
	sk_X509_pop_free(others, X509_free);
}