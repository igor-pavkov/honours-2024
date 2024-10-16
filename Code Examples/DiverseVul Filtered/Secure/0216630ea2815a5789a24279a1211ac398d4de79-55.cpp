PHP_FUNCTION(openssl_pkey_export_to_file)
{
	struct php_x509_request req;
	zval * zpkey, * args = NULL;
	char * passphrase = NULL;
	size_t passphrase_len = 0;
	char * filename = NULL;
	size_t filename_len = 0;
	zend_resource *key_resource = NULL;
	int pem_write = 0;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	const EVP_CIPHER * cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zp|s!a!", &zpkey, &filename, &filename_len, &passphrase, &passphrase_len, &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase);
	key = php_openssl_evp_from_zval(zpkey, 0, passphrase, passphrase_len, 0, &key_resource);

	if (key == NULL) {
		php_error_docref(NULL, E_WARNING, "cannot get key from parameter 1");
		RETURN_FALSE;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		RETURN_FALSE;
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (bio_out == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}

		if (passphrase && req.priv_key_encrypt) {
			if (req.priv_key_encrypt_cipher) {
				cipher = req.priv_key_encrypt_cipher;
			} else {
				cipher = (EVP_CIPHER *) EVP_des_ede3_cbc();
			}
		} else {
			cipher = NULL;
		}

		switch (EVP_PKEY_base_id(key)) {
#ifdef HAVE_EVP_PKEY_EC
			case EVP_PKEY_EC:
				pem_write = PEM_write_bio_ECPrivateKey(
						bio_out, EVP_PKEY_get0_EC_KEY(key), cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
#endif
			default:
				pem_write = PEM_write_bio_PrivateKey(
						bio_out, key, cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
		}

		if (pem_write) {
			/* Success!
			 * If returning the output as a string, do so now */
			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
		}
	}

clean_exit:
	PHP_SSL_REQ_DISPOSE(&req);

	if (key_resource == NULL && key) {
		EVP_PKEY_free(key);
	}
	if (bio_out) {
		BIO_free(bio_out);
	}
}