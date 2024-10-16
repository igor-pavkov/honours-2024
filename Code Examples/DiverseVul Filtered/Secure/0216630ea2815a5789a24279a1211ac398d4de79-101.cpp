PHP_FUNCTION(openssl_pkcs7_read)
{
	zval * zout = NULL, zcert;
	char *p7b;
	size_t p7b_len;
	STACK_OF(X509) *certs = NULL;
	STACK_OF(X509_CRL) *crls = NULL;
	BIO * bio_in = NULL, * bio_out = NULL;
	PKCS7 * p7 = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/", &p7b, &p7b_len,
				&zout) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(p7b_len, p7b);

	bio_in = BIO_new(BIO_s_mem());
	if (bio_in == NULL) {
		goto clean_exit;
	}

	if (0 >= BIO_write(bio_in, p7b, (int)p7b_len)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	p7 = PEM_read_bio_PKCS7(bio_in, NULL, NULL, NULL);
	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	switch (OBJ_obj2nid(p7->type)) {
		case NID_pkcs7_signed:
			if (p7->d.sign != NULL) {
				certs = p7->d.sign->cert;
				crls = p7->d.sign->crl;
			}
			break;
		case NID_pkcs7_signedAndEnveloped:
			if (p7->d.signed_and_enveloped != NULL) {
				certs = p7->d.signed_and_enveloped->cert;
				crls = p7->d.signed_and_enveloped->crl;
			}
			break;
		default:
			break;
	}

	zval_dtor(zout);
	array_init(zout);

	if (certs != NULL) {
		for (i = 0; i < sk_X509_num(certs); i++) {
			X509* ca = sk_X509_value(certs, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509(bio_out, ca)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	if (crls != NULL) {
		for (i = 0; i < sk_X509_CRL_num(crls); i++) {
			X509_CRL* crl = sk_X509_CRL_value(crls, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509_CRL(bio_out, crl)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	RETVAL_TRUE;

clean_exit:
	if (bio_in != NULL) {
		BIO_free(bio_in);
	}

	if (p7 != NULL) {
		PKCS7_free(p7);
	}
}