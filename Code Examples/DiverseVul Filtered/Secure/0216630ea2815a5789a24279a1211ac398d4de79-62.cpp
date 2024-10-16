static EVP_PKEY * php_openssl_evp_from_zval(
		zval * val, int public_key, char *passphrase, size_t passphrase_len,
		int makeresource, zend_resource **resourceval)
{
	EVP_PKEY * key = NULL;
	X509 * cert = NULL;
	int free_cert = 0;
	zend_resource *cert_res = NULL;
	char * filename = NULL;
	zval tmp;

	ZVAL_NULL(&tmp);

#define TMP_CLEAN \
	if (Z_TYPE(tmp) == IS_STRING) {\
		zval_dtor(&tmp); \
	} \
	return NULL;

	if (resourceval) {
		*resourceval = NULL;
	}
	if (Z_TYPE_P(val) == IS_ARRAY) {
		zval * zphrase;

		/* get passphrase */

		if ((zphrase = zend_hash_index_find(Z_ARRVAL_P(val), 1)) == NULL) {
			php_error_docref(NULL, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			return NULL;
		}

		if (Z_TYPE_P(zphrase) == IS_STRING) {
			passphrase = Z_STRVAL_P(zphrase);
			passphrase_len = Z_STRLEN_P(zphrase);
		} else {
			ZVAL_COPY(&tmp, zphrase);
			convert_to_string(&tmp);
			passphrase = Z_STRVAL(tmp);
			passphrase_len = Z_STRLEN(tmp);
		}

		/* now set val to be the key param and continue */
		if ((val = zend_hash_index_find(Z_ARRVAL_P(val), 0)) == NULL) {
			php_error_docref(NULL, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			TMP_CLEAN;
		}
	}

	if (Z_TYPE_P(val) == IS_RESOURCE) {
		void * what;
		zend_resource * res = Z_RES_P(val);

		what = zend_fetch_resource2(res, "OpenSSL X.509/key", le_x509, le_key);
		if (!what) {
			TMP_CLEAN;
		}
		if (resourceval) {
			*resourceval = res;
			Z_ADDREF_P(val);
		}
		if (res->type == le_x509) {
			/* extract key from cert, depending on public_key param */
			cert = (X509*)what;
			free_cert = 0;
		} else if (res->type == le_key) {
			int is_priv;

			is_priv = php_openssl_is_private_key((EVP_PKEY*)what);

			/* check whether it is actually a private key if requested */
			if (!public_key && !is_priv) {
				php_error_docref(NULL, E_WARNING, "supplied key param is a public key");
				TMP_CLEAN;
			}

			if (public_key && is_priv) {
				php_error_docref(NULL, E_WARNING, "Don't know how to get public key from this private key");
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
		if (!(Z_TYPE_P(val) == IS_STRING || Z_TYPE_P(val) == IS_OBJECT)) {
			TMP_CLEAN;
		}
		convert_to_string_ex(val);

		if (Z_STRLEN_P(val) > 7 && memcmp(Z_STRVAL_P(val), "file://", sizeof("file://") - 1) == 0) {
			filename = Z_STRVAL_P(val) + (sizeof("file://") - 1);
			if (php_openssl_open_base_dir_chk(filename)) {
				TMP_CLEAN;
			}
		}
		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key) {
			cert = php_openssl_x509_from_zval(val, 0, &cert_res);
			free_cert = (cert_res == NULL);
			/* actual extraction done later */
			if (!cert) {
				/* not a X509 certificate, try to retrieve public key */
				BIO* in;
				if (filename) {
					in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
				} else {
					in = BIO_new_mem_buf(Z_STRVAL_P(val), (int)Z_STRLEN_P(val));
				}
				if (in == NULL) {
					php_openssl_store_errors();
					TMP_CLEAN;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (filename) {
				in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
			} else {
				in = BIO_new_mem_buf(Z_STRVAL_P(val), (int)Z_STRLEN_P(val));
			}

			if (in == NULL) {
				TMP_CLEAN;
			}
			if (passphrase == NULL) {
				key = PEM_read_bio_PrivateKey(in, NULL, NULL, NULL);
			} else {
				struct php_openssl_pem_password password;
				password.key = passphrase;
				password.len = passphrase_len;
				key = PEM_read_bio_PrivateKey(in, NULL, php_openssl_pem_password_cb, &password);
			}
			BIO_free(in);
		}
	}

	if (key == NULL) {
		php_openssl_store_errors();

		if (public_key && cert) {
			/* extract public key from X509 cert */
			key = (EVP_PKEY *) X509_get_pubkey(cert);
			if (key == NULL) {
				php_openssl_store_errors();
			}
		}
	}

	if (free_cert && cert) {
		X509_free(cert);
	}
	if (key && makeresource && resourceval) {
		*resourceval = zend_register_resource(key, le_key);
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		zval_dtor(&tmp);
	}
	return key;
}