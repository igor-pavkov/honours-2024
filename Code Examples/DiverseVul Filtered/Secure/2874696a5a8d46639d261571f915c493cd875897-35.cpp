static EVP_PKEY * php_openssl_evp_from_zval(zval ** val, int public_key, char * passphrase, int makeresource, long * resourceval TSRMLS_DC)
{
	EVP_PKEY * key = NULL;
	X509 * cert = NULL;
	int free_cert = 0;
	long cert_res = -1;
	char * filename = NULL;
	zval tmp;

	Z_TYPE(tmp) = IS_NULL;

#define TMP_CLEAN \
	if (Z_TYPE(tmp) == IS_STRING) {\
		zval_dtor(&tmp); \
	} \
	return NULL;

	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_ARRAY) {
		zval ** zphrase;

		/* get passphrase */

		if (zend_hash_index_find(HASH_OF(*val), 1, (void **)&zphrase) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			return NULL;
		}

		if (Z_TYPE_PP(zphrase) == IS_STRING) {
			passphrase = Z_STRVAL_PP(zphrase);
		} else {
			tmp = **zphrase;
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
			passphrase = Z_STRVAL(tmp);
		}

		/* now set val to be the key param and continue */
		if (zend_hash_index_find(HASH_OF(*val), 0, (void **)&val) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			TMP_CLEAN;
		}
	}

	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509/key", &type, 2, le_x509, le_key);
		if (!what) {
			TMP_CLEAN;
		}
		if (resourceval) {
			*resourceval = Z_LVAL_PP(val);
		}
		if (type == le_x509) {
			/* extract key from cert, depending on public_key param */
			cert = (X509*)what;
			free_cert = 0;
		} else if (type == le_key) {
			int is_priv;

			is_priv = php_openssl_is_private_key((EVP_PKEY*)what TSRMLS_CC);

			/* check whether it is actually a private key if requested */
			if (!public_key && !is_priv) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param is a public key");
				TMP_CLEAN;
			}

			if (public_key && is_priv) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Don't know how to get public key from this private key");
				TMP_CLEAN;
			} else {
				if (Z_TYPE(tmp) == IS_STRING) {
					zval_dtor(&tmp);
				}
				/* got the key - return it */
				return (EVP_PKEY*)what;
			}
		} else {
			/* other types could be used here - eg: file pointers and read in the data from them */
			TMP_CLEAN;
		}
	} else {
		/* force it to be a string and check if it refers to a file */
		/* passing non string values leaks, object uses toString, it returns NULL
		 * See bug38255.phpt
		 */
		if (!(Z_TYPE_PP(val) == IS_STRING || Z_TYPE_PP(val) == IS_OBJECT)) {
			TMP_CLEAN;
		}
		convert_to_string_ex(val);

		if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
			filename = Z_STRVAL_PP(val) + (sizeof("file://") - 1);
		}
		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key) {
			cert = php_openssl_x509_from_zval(val, 0, &cert_res TSRMLS_CC);
			free_cert = (cert_res == -1);
			/* actual extraction done later */
			if (!cert) {
				/* not a X509 certificate, try to retrieve public key */
				BIO* in;
				if (filename) {
					in = BIO_new_file(filename, "r");
				} else {
					in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				}
				if (in == NULL) {
					TMP_CLEAN;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (filename) {
				if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
					TMP_CLEAN;
				}
				in = BIO_new_file(filename, "r");
			} else {
				in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
			}

			if (in == NULL) {
				TMP_CLEAN;
			}
			key = PEM_read_bio_PrivateKey(in, NULL,NULL, passphrase);
			BIO_free(in);
		}
	}

	if (public_key && cert && key == NULL) {
		/* extract public key from X509 cert */
		key = (EVP_PKEY *) X509_get_pubkey(cert);
	}

	if (free_cert && cert) {
		X509_free(cert);
	}
	if (key && makeresource && resourceval) {
		*resourceval = ZEND_REGISTER_RESOURCE(NULL, key, le_key);
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		zval_dtor(&tmp);
	}
	return key;
}