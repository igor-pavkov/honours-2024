PHP_FUNCTION(openssl_pkcs12_read)
{
	zval *zout = NULL, zextracerts, zcert, zpkey;
	char *pass, *zp12;
	size_t pass_len, zp12_len;
	PKCS12 * p12 = NULL;
	EVP_PKEY * pkey = NULL;
	X509 * cert = NULL;
	STACK_OF(X509) * ca = NULL;
	BIO * bio_in = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/s", &zp12, &zp12_len, &zout, &pass, &pass_len) == FAILURE)
		return;

	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(zp12_len, pkcs12);

	bio_in = BIO_new(BIO_s_mem());

	if (0 >= BIO_write(bio_in, zp12, (int)zp12_len)) {
		php_openssl_store_errors();
		goto cleanup;
	}

	if (d2i_PKCS12_bio(bio_in, &p12) && PKCS12_parse(p12, pass, &pkey, &cert, &ca)) {
		BIO * bio_out;
		int cert_num;

		zval_dtor(zout);
		array_init(zout);

		if (cert) {
			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_X509(bio_out, cert)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_assoc_zval(zout, "cert", &zcert);
			} else {
				php_openssl_store_errors();
			}
			BIO_free(bio_out);
		}

		if (pkey) {
			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_PrivateKey(bio_out, pkey, NULL, NULL, 0, 0, NULL)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zpkey, bio_buf->data, bio_buf->length);
				add_assoc_zval(zout, "pkey", &zpkey);
			} else {
				php_openssl_store_errors();
			}
			BIO_free(bio_out);
		}

		cert_num = sk_X509_num(ca);
		if (ca && cert_num) {
			array_init(&zextracerts);

			for (i = 0; i < cert_num; i++) {
				zval zextracert;
				X509* aCA = sk_X509_pop(ca);
				if (!aCA) break;

				bio_out = BIO_new(BIO_s_mem());
				if (PEM_write_bio_X509(bio_out, aCA)) {
					BUF_MEM *bio_buf;
					BIO_get_mem_ptr(bio_out, &bio_buf);
					ZVAL_STRINGL(&zextracert, bio_buf->data, bio_buf->length);
					add_index_zval(&zextracerts, i, &zextracert);
				}

				X509_free(aCA);
				BIO_free(bio_out);
			}

			sk_X509_free(ca);
			add_assoc_zval(zout, "extracerts", &zextracerts);
		}

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	cleanup:
	if (bio_in) {
		BIO_free(bio_in);
	}
	if (pkey) {
		EVP_PKEY_free(pkey);
	}
	if (cert) {
		X509_free(cert);
	}
	if (p12) {
		PKCS12_free(p12);
	}
}