static X509_REQ * php_openssl_csr_from_zval(zval * val, int makeresource, zend_resource **resourceval)
{
	X509_REQ * csr = NULL;
	char * filename = NULL;
	BIO * in;

	if (resourceval) {
		*resourceval = NULL;
	}
	if (Z_TYPE_P(val) == IS_RESOURCE) {
		void * what;
		zend_resource *res = Z_RES_P(val);

		what = zend_fetch_resource(res, "OpenSSL X.509 CSR", le_csr);
		if (what) {
			if (resourceval) {
				*resourceval = res;
				if (makeresource) {
					Z_ADDREF_P(val);
				}
			}
			return (X509_REQ*)what;
		}
		return NULL;
	} else if (Z_TYPE_P(val) != IS_STRING) {
		return NULL;
	}

	if (Z_STRLEN_P(val) > 7 && memcmp(Z_STRVAL_P(val), "file://", sizeof("file://") - 1) == 0) {
		filename = Z_STRVAL_P(val) + (sizeof("file://") - 1);
	}
	if (filename) {
		if (php_openssl_open_base_dir_chk(filename)) {
			return NULL;
		}
		in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
	} else {
		in = BIO_new_mem_buf(Z_STRVAL_P(val), (int)Z_STRLEN_P(val));
	}

	if (in == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	csr = PEM_read_bio_X509_REQ(in, NULL,NULL,NULL);
	if (csr == NULL) {
		php_openssl_store_errors();
	}

	BIO_free(in);

	return csr;
}