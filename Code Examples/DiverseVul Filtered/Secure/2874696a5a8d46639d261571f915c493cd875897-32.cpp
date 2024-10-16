PHP_FUNCTION(openssl_pkcs12_read)
{
	zval *zout = NULL, *zextracerts, *zcert, *zpkey;
	char *pass, *zp12;
	int pass_len, zp12_len;
	PKCS12 * p12 = NULL;
	EVP_PKEY * pkey = NULL;
	X509 * cert = NULL;
	STACK_OF(X509) * ca = NULL;
	BIO * bio_in = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szs", &zp12, &zp12_len, &zout, &pass, &pass_len) == FAILURE)
		return;

	RETVAL_FALSE;

	bio_in = BIO_new(BIO_s_mem());

	if(!BIO_write(bio_in, zp12, zp12_len))
		goto cleanup;

	if(d2i_PKCS12_bio(bio_in, &p12)) {
		if(PKCS12_parse(p12, pass, &pkey, &cert, &ca)) {
			BIO * bio_out;

			zval_dtor(zout);
			array_init(zout);

			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_X509(bio_out, cert)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				MAKE_STD_ZVAL(zcert);
				ZVAL_STRINGL(zcert, bio_buf->data, bio_buf->length, 1);
				add_assoc_zval(zout, "cert", zcert);
			}
			BIO_free(bio_out);

			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_PrivateKey(bio_out, pkey, NULL, NULL, 0, 0, NULL)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				MAKE_STD_ZVAL(zpkey);
				ZVAL_STRINGL(zpkey, bio_buf->data, bio_buf->length, 1);
				add_assoc_zval(zout, "pkey", zpkey);
			}
			BIO_free(bio_out);

			MAKE_STD_ZVAL(zextracerts);
			array_init(zextracerts);

			for (i=0;;i++) {
				zval * zextracert;
				X509* aCA = sk_X509_pop(ca);
				if (!aCA) break;

				bio_out = BIO_new(BIO_s_mem());
				if (PEM_write_bio_X509(bio_out, aCA)) {
					BUF_MEM *bio_buf;
					BIO_get_mem_ptr(bio_out, &bio_buf);
					MAKE_STD_ZVAL(zextracert);
					ZVAL_STRINGL(zextracert, bio_buf->data, bio_buf->length, 1);
					add_index_zval(zextracerts, i, zextracert);

				}
				BIO_free(bio_out);

				X509_free(aCA);
			}
			if(ca) {
				sk_X509_free(ca);
				add_assoc_zval(zout, "extracerts", zextracerts);
			} else {
				zval_dtor(zextracerts);
			}

			RETVAL_TRUE;

			PKCS12_free(p12);
		}
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
}