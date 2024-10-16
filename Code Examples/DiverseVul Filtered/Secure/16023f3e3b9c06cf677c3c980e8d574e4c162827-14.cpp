SSL *php_SSL_new_from_context(SSL_CTX *ctx, php_stream *stream TSRMLS_DC) /* {{{ */
{
	zval **val = NULL;
	char *cafile = NULL;
	char *capath = NULL;
	char *certfile = NULL;
	char *cipherlist = NULL;
	int ok = 1;

	ERR_clear_error();

	/* look at context options in the stream and set appropriate verification flags */
	if (GET_VER_OPT("verify_peer") && zval_is_true(*val)) {

		/* turn on verification callback */
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);

		/* CA stuff */
		GET_VER_OPT_STRING("cafile", cafile);
		GET_VER_OPT_STRING("capath", capath);

		if (cafile || capath) {
			if (!SSL_CTX_load_verify_locations(ctx, cafile, capath)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set verify locations `%s' `%s'", cafile, capath);
				return NULL;
			}
		}

		if (GET_VER_OPT("verify_depth")) {
			convert_to_long_ex(val);
			SSL_CTX_set_verify_depth(ctx, Z_LVAL_PP(val));
		}
	} else {
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	}

	/* callback for the passphrase (for localcert) */
	if (GET_VER_OPT("passphrase")) {
		SSL_CTX_set_default_passwd_cb_userdata(ctx, stream);
		SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
	}

	GET_VER_OPT_STRING("ciphers", cipherlist);
	if (!cipherlist) {
		cipherlist = "DEFAULT";
	}
	if (SSL_CTX_set_cipher_list(ctx, cipherlist) != 1) {
		return NULL;
	}

	GET_VER_OPT_STRING("local_cert", certfile);
	if (certfile) {
		char resolved_path_buff[MAXPATHLEN];
		const char * private_key = NULL;

		if (VCWD_REALPATH(certfile, resolved_path_buff)) {
			/* a certificate to use for authentication */
			if (SSL_CTX_use_certificate_chain_file(ctx, resolved_path_buff) != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set local cert chain file `%s'; Check that your cafile/capath settings include details of your certificate and its issuer", certfile);
				return NULL;
			}
			GET_VER_OPT_STRING("local_pk", private_key);

			if (private_key) {
				char resolved_path_buff_pk[MAXPATHLEN];
				if (VCWD_REALPATH(private_key, resolved_path_buff_pk)) {
					if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff_pk, SSL_FILETYPE_PEM) != 1) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set private key file `%s'", resolved_path_buff_pk);
						return NULL;
					}
				}
			} else {
				if (SSL_CTX_use_PrivateKey_file(ctx, resolved_path_buff, SSL_FILETYPE_PEM) != 1) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set private key file `%s'", resolved_path_buff);
					return NULL;
				}
			}

#if OPENSSL_VERSION_NUMBER < 0x10001001L
			do {
				/* Unnecessary as of OpenSSLv1.0.1 (will segfault if used with >= 10001001 ) */
				X509 *cert = NULL;
				EVP_PKEY *key = NULL;
				SSL *tmpssl = SSL_new(ctx);
				cert = SSL_get_certificate(tmpssl);

				if (cert) {
					key = X509_get_pubkey(cert);
					EVP_PKEY_copy_parameters(key, SSL_get_privatekey(tmpssl));
					EVP_PKEY_free(key);
				}
				SSL_free(tmpssl);
			} while (0);
#endif
			if (!SSL_CTX_check_private_key(ctx)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Private key does not match certificate!");
			}
		}
	}
	if (ok) {
		SSL *ssl = SSL_new(ctx);

		if (ssl) {
			/* map SSL => stream */
			SSL_set_ex_data(ssl, ssl_stream_data_index, stream);
		}
		return ssl;
	}

	return NULL;
}