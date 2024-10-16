static php_stream* http_connect(zval* this_ptr, php_url *phpurl, int use_ssl, php_stream_context *context, int *use_proxy)
{
	php_stream *stream;
	zval *proxy_host, *proxy_port, *tmp;
	char *host;
	char *name;
	char *protocol;
	zend_long namelen;
	int port;
	int old_error_reporting;
	struct timeval tv;
	struct timeval *timeout = NULL;

	if ((proxy_host = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_proxy_host", sizeof("_proxy_host")-1)) != NULL &&
	    Z_TYPE_P(proxy_host) == IS_STRING &&
	    (proxy_port = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_proxy_port", sizeof("_proxy_port")-1)) != NULL &&
	    Z_TYPE_P(proxy_port) == IS_LONG) {
		host = Z_STRVAL_P(proxy_host);
		port = Z_LVAL_P(proxy_port);
		*use_proxy = 1;
	} else {
		host = phpurl->host;
		port = phpurl->port;
	}
	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_connection_timeout", sizeof("_connection_timeout")-1)) != NULL &&
	    Z_TYPE_P(tmp) == IS_LONG && Z_LVAL_P(tmp) > 0) {
	  tv.tv_sec = Z_LVAL_P(tmp);
	  tv.tv_usec = 0;
		timeout = &tv;
	}

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	/* Changed ternary operator to an if/else so that additional comparisons can be done on the ssl_method property */
	if (use_ssl && !*use_proxy) {
		if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_ssl_method", sizeof("_ssl_method")-1)) != NULL &&
			Z_TYPE_P(tmp) == IS_LONG) {
			/* uses constants declared in soap.c to determine ssl uri protocol */
			switch (Z_LVAL_P(tmp)) {
				case SOAP_SSL_METHOD_TLS:
					protocol = "tls";
					break;

				case SOAP_SSL_METHOD_SSLv2:
					protocol = "sslv2";
					break;

				case SOAP_SSL_METHOD_SSLv3:
					protocol = "sslv3";
					break;

				case SOAP_SSL_METHOD_SSLv23:
					protocol = "ssl";
					break;

				default:
					protocol = "ssl";
					break;

			}
		} else {
			protocol = "ssl";
		}
	} else {
		protocol = "tcp";
	}

	namelen = spprintf(&name, 0, "%s://%s:%d", protocol, host, port);

	stream = php_stream_xport_create(name, namelen,
		REPORT_ERRORS,
		STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
		NULL /*persistent_id*/,
		timeout,
		context,
		NULL, NULL);
	efree(name);

	/* SSL & proxy */
	if (stream && *use_proxy && use_ssl) {
		smart_str soap_headers = {0};

		smart_str_append_const(&soap_headers, "CONNECT ");
		smart_str_appends(&soap_headers, phpurl->host);
		smart_str_appendc(&soap_headers, ':');
		smart_str_append_unsigned(&soap_headers, phpurl->port);
		smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != 80) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		smart_str_append_const(&soap_headers, "\r\n");
		proxy_authentication(this_ptr, &soap_headers);
		smart_str_append_const(&soap_headers, "\r\n");
		if (php_stream_write(stream, ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s)) != ZSTR_LEN(soap_headers.s)) {
			php_stream_close(stream);
			stream = NULL;
		}
 	 	smart_str_free(&soap_headers);

 	 	if (stream) {
			zend_string *http_headers = get_http_headers(stream);
			if (http_headers) {
				zend_string_free(http_headers);
			} else {
				php_stream_close(stream);
				stream = NULL;
			}
		}
		/* enable SSL transport layer */
		if (stream) {
			/* if a stream is created without encryption, check to see if SSL method parameter is specified and use
 			   proper encrypyion method based on constants defined in soap.c */
			int crypto_method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
			if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_ssl_method", sizeof("_ssl_method")-1)) != NULL &&
				Z_TYPE_P(tmp) == IS_LONG) {
				switch (Z_LVAL_P(tmp)) {
					case SOAP_SSL_METHOD_TLS:
						crypto_method = STREAM_CRYPTO_METHOD_TLS_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv2:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv2_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv3:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv23:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
						break;

					default:
						crypto_method = STREAM_CRYPTO_METHOD_TLS_CLIENT;
						break;
				}
			}
			if (php_stream_xport_crypto_setup(stream, crypto_method, NULL) < 0 ||
			    php_stream_xport_crypto_enable(stream, 1) < 0) {
				php_stream_close(stream);
				stream = NULL;
			}
		}
	}

	EG(error_reporting) = old_error_reporting;
	return stream;
}