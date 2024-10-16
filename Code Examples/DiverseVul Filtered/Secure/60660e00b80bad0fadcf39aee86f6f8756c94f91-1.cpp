int ssl_connect(struct tunnel *tunnel)
{
	ssl_disconnect(tunnel);

	tunnel->ssl_socket = tcp_connect(tunnel);
	if (tunnel->ssl_socket == -1)
		return 1;

	// registration is deprecated from OpenSSL 1.1.0 onward
#if OPENSSL_API_COMPAT < 0x10100000L
	// Register the error strings for libcrypto & libssl
	SSL_load_error_strings();
	// Register the available ciphers and digests
	SSL_library_init();
#endif

	tunnel->ssl_context = SSL_CTX_new(SSLv23_client_method());
	if (tunnel->ssl_context == NULL) {
		log_error("SSL_CTX_new: %s\n",
		          ERR_error_string(ERR_peek_last_error(), NULL));
		return 1;
	}

	// Load the OS default CA files
	if (!SSL_CTX_set_default_verify_paths(tunnel->ssl_context))
		log_error("Could not load OS OpenSSL files.\n");

	if (tunnel->config->ca_file) {
		if (!SSL_CTX_load_verify_locations(
		            tunnel->ssl_context,
		            tunnel->config->ca_file, NULL)) {
			log_error("SSL_CTX_load_verify_locations: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}
	}

	/* Use engine for PIV if user-cert config starts with pkcs11 URI: */
	if (tunnel->config->use_engine > 0) {

		ENGINE *e;
		ENGINE_load_builtin_engines();
		e = ENGINE_by_id("pkcs11");
		if (!e) {
			log_error("Could not load pkcs11 Engine: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}
		if (!ENGINE_init(e)) {
			log_error("Could not init pkcs11 Engine: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			ENGINE_free(e);
			return 1;
		}
		if (!ENGINE_set_default_RSA(e))
			abort();

		ENGINE_finish(e);
		ENGINE_free(e);

		struct token parms;
		parms.uri = tunnel->config->user_cert;
		parms.cert = NULL;

		if (!ENGINE_ctrl_cmd(e, "LOAD_CERT_CTRL", 0, &parms, NULL, 1)) {
			log_error("PKCS11 ENGINE_ctrl_cmd: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}

		if (!SSL_CTX_use_certificate(tunnel->ssl_context, parms.cert)) {
			log_error("PKCS11 SSL_CTX_use_certificate: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}

		EVP_PKEY * privkey = ENGINE_load_private_key(
		                             e, parms.uri, UI_OpenSSL(), NULL);
		if (!privkey) {
			log_error("PKCS11 ENGINE_load_private_key: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}

		if (!SSL_CTX_use_PrivateKey(tunnel->ssl_context, privkey)) {
			log_error("PKCS11 SSL_CTX_use_PrivateKey_file: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}

		if (!SSL_CTX_check_private_key(tunnel->ssl_context)) {
			log_error("PKCS11 SSL_CTX_check_private_key: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}

	} else {        /* end PKCS11-engine */

		if (tunnel->config->user_cert) {
			if (!SSL_CTX_use_certificate_file(
			            tunnel->ssl_context, tunnel->config->user_cert,
			            SSL_FILETYPE_PEM)) {
				log_error("SSL_CTX_use_certificate_file: %s\n",
				          ERR_error_string(ERR_peek_last_error(), NULL));
				return 1;
			}
		}

		if (tunnel->config->user_key) {
			if (!SSL_CTX_use_PrivateKey_file(
			            tunnel->ssl_context, tunnel->config->user_key,
			            SSL_FILETYPE_PEM)) {
				log_error("SSL_CTX_use_PrivateKey_file: %s\n",
				          ERR_error_string(ERR_peek_last_error(), NULL));
				return 1;
			}
		}

		if (tunnel->config->user_cert && tunnel->config->user_key) {
			if (!SSL_CTX_check_private_key(tunnel->ssl_context)) {
				log_error("SSL_CTX_check_private_key: %s\n",
				          ERR_error_string(ERR_peek_last_error(), NULL));
				return 1;
			}
		}
	}

	if (!tunnel->config->insecure_ssl) {
		long sslctxopt = SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
		long checkopt;

		checkopt = SSL_CTX_set_options(tunnel->ssl_context, sslctxopt);
		if ((checkopt & sslctxopt) != sslctxopt) {
			log_error("SSL_CTX_set_options didn't set opt: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}
	}

	tunnel->ssl_handle = SSL_new(tunnel->ssl_context);
	if (tunnel->ssl_handle == NULL) {
		log_error("SSL_new: %s\n",
		          ERR_error_string(ERR_peek_last_error(), NULL));
		return 1;
	}

	if (!tunnel->config->insecure_ssl) {
		if (!tunnel->config->cipher_list) {
			const char *cipher_list;
			if (tunnel->config->seclevel_1)
				cipher_list = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4@SECLEVEL=1";
			else
				cipher_list = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
			tunnel->config->cipher_list = strdup(cipher_list);
		}
	} else {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		if (tunnel->config->min_tls <= 0)
			tunnel->config->min_tls = TLS1_VERSION;
#endif
		if (!tunnel->config->cipher_list && tunnel->config->seclevel_1) {
			const char *cipher_list = "DEFAULT@SECLEVEL=1";
			tunnel->config->cipher_list = strdup(cipher_list);
		}
	}

	if (tunnel->config->cipher_list) {
		log_debug("Setting cipher list to: %s\n", tunnel->config->cipher_list);
		if (!SSL_set_cipher_list(tunnel->ssl_handle,
		                         tunnel->config->cipher_list)) {
			log_error("SSL_set_cipher_list failed: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}
	}

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	if (tunnel->config->min_tls > 0) {
		log_debug("Setting min proto version to: 0x%x\n",
		          tunnel->config->min_tls);
		if (!SSL_set_min_proto_version(tunnel->ssl_handle,
		                               tunnel->config->min_tls)) {
			log_error("SSL_set_min_proto_version failed: %s\n",
			          ERR_error_string(ERR_peek_last_error(), NULL));
			return 1;
		}
	}
#endif

	if (!SSL_set_fd(tunnel->ssl_handle, tunnel->ssl_socket)) {
		log_error("SSL_set_fd: %s\n",
		          ERR_error_string(ERR_peek_last_error(), NULL));
		return 1;
	}
	SSL_set_mode(tunnel->ssl_handle, SSL_MODE_AUTO_RETRY);

	// Initiate SSL handshake
	if (SSL_connect(tunnel->ssl_handle) != 1) {
		log_error("SSL_connect: %s\n"
		          "You might want to try --insecure-ssl or specify a different --cipher-list\n",
		          ERR_error_string(ERR_peek_last_error(), NULL));
		return 1;
	}
	SSL_set_mode(tunnel->ssl_handle, SSL_MODE_AUTO_RETRY);

	if (ssl_verify_cert(tunnel))
		return 1;

	// Disable SIGPIPE (occurs when trying to write to an already-closed
	// socket).
	signal(SIGPIPE, SIG_IGN);

	return 0;
}