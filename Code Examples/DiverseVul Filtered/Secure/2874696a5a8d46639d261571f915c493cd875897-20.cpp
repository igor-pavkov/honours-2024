static X509 * php_openssl_x509_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC)
{
	X509 *cert = NULL;

	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		/* is it an x509 resource ? */
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509", &type, 1, le_x509);
		if (!what) {
			return NULL;
		}
		/* this is so callers can decide if they should free the X509 */
		if (resourceval) {
			*resourceval = Z_LVAL_PP(val);
		}
		if (type == le_x509) {
			return (X509*)what;
		}
		/* other types could be used here - eg: file pointers and read in the data from them */

		return NULL;
	}

	if (!(Z_TYPE_PP(val) == IS_STRING || Z_TYPE_PP(val) == IS_OBJECT)) {
		return NULL;
	}

	/* force it to be a string and check if it refers to a file */
	convert_to_string_ex(val);

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
		/* read cert from the named file */
		BIO *in;

		if (php_openssl_open_base_dir_chk(Z_STRVAL_PP(val) + (sizeof("file://") - 1) TSRMLS_CC)) {
			return NULL;
		}

		in = BIO_new_file(Z_STRVAL_PP(val) + (sizeof("file://") - 1), "r");
		if (in == NULL) {
			return NULL;
		}
		cert = PEM_read_bio_X509(in, NULL, NULL, NULL);
		BIO_free(in);
	} else {
		BIO *in;

		in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
		if (in == NULL) {
			return NULL;
		}
#ifdef TYPEDEF_D2I_OF
		cert = (X509 *) PEM_ASN1_read_bio((d2i_of_void *)d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#else
		cert = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#endif
		BIO_free(in);
	}

	if (cert && makeresource && resourceval) {
		*resourceval = zend_list_insert(cert, le_x509 TSRMLS_CC);
	}
	return cert;
}