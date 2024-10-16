PHP_FUNCTION(openssl_pkcs7_encrypt)
{
	zval * zrecipcerts, * zheaders = NULL;
	STACK_OF(X509) * recipcerts = NULL;
	BIO * infile = NULL, * outfile = NULL;
	zend_long flags = 0;
	PKCS7 * p7 = NULL;
	zval * zcertval;
	X509 * cert;
	const EVP_CIPHER *cipher = NULL;
	zend_long cipherid = PHP_OPENSSL_CIPHER_DEFAULT;
	zend_string * strindex;
	char * infilename = NULL;
	size_t infilename_len;
	char * outfilename = NULL;
	size_t outfilename_len;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppza!|ll", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags, &cipherid) == FAILURE)
		return;


	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		return;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(flags));
	if (outfile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zrecipcerts), zcertval) {
			zend_resource *certresource;

			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (certresource != NULL) {
				/* we shouldn't free this particular cert, as it is a resource.
					make a copy and push that on the stack instead */
				cert = X509_dup(cert);
				if (cert == NULL) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			sk_X509_push(recipcerts, cert);
		} ZEND_HASH_FOREACH_END();
	} else {
		/* a single certificate */
		zend_resource *certresource;

		cert = php_openssl_x509_from_zval(zrecipcerts, 0, &certresource);
		if (cert == NULL) {
			goto clean_exit;
		}

		if (certresource != NULL) {
			/* we shouldn't free this particular cert, as it is a resource.
				make a copy and push that on the stack instead */
			cert = X509_dup(cert);
			if (cert == NULL) {
				php_openssl_store_errors();
				goto clean_exit;
			}
		}
		sk_X509_push(recipcerts, cert);
	}

	/* sanity check the cipher */
	cipher = php_openssl_get_evp_cipher_from_algo(cipherid);
	if (cipher == NULL) {
		/* shouldn't happen */
		php_error_docref(NULL, E_WARNING, "Failed to get cipher");
		goto clean_exit;
	}

	p7 = PKCS7_encrypt(recipcerts, infile, (EVP_CIPHER*)cipher, (int)flags);

	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	/* tack on extra headers */
	if (zheaders) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, zcertval) {
			convert_to_string_ex(zcertval);

			if (strindex) {
				BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), Z_STRVAL_P(zcertval));
			} else {
				BIO_printf(outfile, "%s\n", Z_STRVAL_P(zcertval));
			}
		} ZEND_HASH_FOREACH_END();
	}

	(void)BIO_reset(infile);

	/* write the encrypted data */
	if (!SMIME_write_PKCS7(outfile, p7, infile, (int)flags)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (recipcerts) {
		sk_X509_pop_free(recipcerts, X509_free);
	}
}