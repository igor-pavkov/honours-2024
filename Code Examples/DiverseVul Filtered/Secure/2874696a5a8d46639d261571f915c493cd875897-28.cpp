static X509_REQ * php_openssl_csr_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC)
{
	X509_REQ * csr = NULL;
	char * filename = NULL;
	BIO * in;

	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509 CSR", &type, 1, le_csr);
		if (what) {
			if (resourceval) {
				*resourceval = Z_LVAL_PP(val);
			}
			return (X509_REQ*)what;
		}
		return NULL;
	} else if (Z_TYPE_PP(val) != IS_STRING) {
		return NULL;
	}

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
		filename = Z_STRVAL_PP(val) + (sizeof("file://") - 1);
	}
	if (filename) {
		if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
			return NULL;
		}
		in = BIO_new_file(filename, "r");
	} else {
		in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
	}
	csr = PEM_read_bio_X509_REQ(in, NULL,NULL,NULL);
	BIO_free(in);

	return csr;
}