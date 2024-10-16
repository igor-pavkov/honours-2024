int make_http_soap_request(zval        *this_ptr,
                           zend_string *buf,
                           char        *location,
                           char        *soapaction,
                           int          soap_version,
                           zval        *return_value)
{
	zend_string *request;
	smart_str soap_headers = {0};
	smart_str soap_headers_z = {0};
	int err;
	php_url *phpurl = NULL;
	php_stream *stream;
	zval *trace, *tmp;
	int use_proxy = 0;
	int use_ssl;
	zend_string *http_body;
	char *content_type, *http_version, *cookie_itt;
	int http_close;
	zend_string *http_headers;
	char *connection;
	int http_1_1;
	int http_status;
	int content_type_xml = 0;
	zend_long redirect_max = 20;
	char *content_encoding;
	char *http_msg = NULL;
	zend_bool old_allow_url_fopen;
	php_stream_context *context = NULL;
	zend_bool has_authorization = 0;
	zend_bool has_proxy_authorization = 0;
	zend_bool has_cookies = 0;

	if (this_ptr == NULL || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		return FALSE;
	}

	request = buf;
	/* Compress request */
	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "compression", sizeof("compression")-1)) != NULL && Z_TYPE_P(tmp) == IS_LONG) {
		int level = Z_LVAL_P(tmp) & 0x0f;
		int kind  = Z_LVAL_P(tmp) & SOAP_COMPRESSION_DEFLATE;

		if (level > 9) {level = 9;}

	  if ((Z_LVAL_P(tmp) & SOAP_COMPRESSION_ACCEPT) != 0) {
			smart_str_append_const(&soap_headers_z,"Accept-Encoding: gzip, deflate\r\n");
	  }
	  if (level > 0) {
			zval func;
			zval retval;
			zval params[3];
			int n;

			ZVAL_STR_COPY(&params[0], buf);
			ZVAL_LONG(&params[1], level);
			if (kind == SOAP_COMPRESSION_DEFLATE) {
				n = 2;
				ZVAL_STRING(&func, "gzcompress");
				smart_str_append_const(&soap_headers_z,"Content-Encoding: deflate\r\n");
			} else {
				n = 3;
				ZVAL_STRING(&func, "gzencode");
				smart_str_append_const(&soap_headers_z,"Content-Encoding: gzip\r\n");
				ZVAL_LONG(&params[2], 0x1f);
			}
			if (call_user_function(CG(function_table), (zval*)NULL, &func, &retval, n, params) == SUCCESS &&
			    Z_TYPE(retval) == IS_STRING) {
				zval_ptr_dtor(&params[0]);
				zval_ptr_dtor(&func);
				request = Z_STR(retval);
			} else {
				zval_ptr_dtor(&params[0]);
				zval_ptr_dtor(&func);
				if (request != buf) {
					zend_string_release(request);
				}
				smart_str_free(&soap_headers_z);
				return FALSE;
			}
	  }
	}

	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1)) != NULL) {
		php_stream_from_zval_no_verify(stream,tmp);
		if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1)) != NULL && Z_TYPE_P(tmp) == IS_LONG) {
			use_proxy = Z_LVAL_P(tmp);
		}
	} else {
		stream = NULL;
	}

	if (location != NULL && location[0] != '\000') {
		phpurl = php_url_parse(location);
	}

	if (NULL != (tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr),
			"_stream_context", sizeof("_stream_context")-1))) {
		context = php_stream_context_from_zval(tmp, 0);
	}

	if (context &&
		(tmp = php_stream_context_get_option(context, "http", "max_redirects")) != NULL) {
		if (Z_TYPE_P(tmp) != IS_STRING || !is_numeric_string(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &redirect_max, NULL, 1)) {
			if (Z_TYPE_P(tmp) == IS_LONG)
				redirect_max = Z_LVAL_P(tmp);
		}
	}

try_again:
	if (phpurl == NULL || phpurl->host == NULL) {
	  if (phpurl != NULL) {php_url_free(phpurl);}
		if (request != buf) {
			zend_string_release(request);
		}
		add_soap_fault(this_ptr, "HTTP", "Unable to parse URL", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	use_ssl = 0;
	if (phpurl->scheme != NULL && strcmp(phpurl->scheme, "https") == 0) {
		use_ssl = 1;
	} else if (phpurl->scheme == NULL || strcmp(phpurl->scheme, "http") != 0) {
		php_url_free(phpurl);
		if (request != buf) {
			zend_string_release(request);
		}
		add_soap_fault(this_ptr, "HTTP", "Unknown protocol. Only http and https are allowed.", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = 1;
	if (use_ssl && php_stream_locate_url_wrapper("https://", NULL, STREAM_LOCATE_WRAPPERS_ONLY) == NULL) {
		php_url_free(phpurl);
		if (request != buf) {
			zend_string_release(request);
		}
		add_soap_fault(this_ptr, "HTTP", "SSL support is not available in this build", NULL, NULL);
		PG(allow_url_fopen) = old_allow_url_fopen;
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (phpurl->port == 0) {
		phpurl->port = use_ssl ? 443 : 80;
	}

	/* Check if request to the same host */
	if (stream != NULL) {
	  php_url *orig;
		if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl")-1)) != NULL &&
		    (orig = (php_url *) zend_fetch_resource_ex(tmp, "httpurl", le_url)) != NULL &&
		    ((use_proxy && !use_ssl) ||
		     (((use_ssl && orig->scheme != NULL && strcmp(orig->scheme, "https") == 0) ||
		      (!use_ssl && orig->scheme == NULL) ||
		      (!use_ssl && strcmp(orig->scheme, "https") != 0)) &&
		     strcmp(orig->host, phpurl->host) == 0 &&
		     orig->port == phpurl->port))) {
    } else {
			php_stream_close(stream);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl")-1);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
			stream = NULL;
			use_proxy = 0;
    }
	}

	/* Check if keep-alive connection is still opened */
	if (stream != NULL && php_stream_eof(stream)) {
		php_stream_close(stream);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl")-1);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
		stream = NULL;
		use_proxy = 0;
	}

	if (!stream) {
		stream = http_connect(this_ptr, phpurl, use_ssl, context, &use_proxy);
		if (stream) {
			php_stream_auto_cleanup(stream);
			add_property_resource(this_ptr, "httpsocket", stream->res);
			GC_REFCOUNT(stream->res)++;
			add_property_long(this_ptr, "_use_proxy", use_proxy);
		} else {
			php_url_free(phpurl);
			if (request != buf) {
				zend_string_release(request);
			}
			add_soap_fault(this_ptr, "HTTP", "Could not connect to host", NULL, NULL);
			PG(allow_url_fopen) = old_allow_url_fopen;
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
	}
	PG(allow_url_fopen) = old_allow_url_fopen;

	if (stream) {
		zval *cookies, *login, *password;
		zend_resource *ret = zend_register_resource(phpurl, le_url);

		add_property_resource(this_ptr, "httpurl", ret);
		GC_REFCOUNT(ret)++;
		/*zend_list_addref(ret);*/

		if (context &&
		    (tmp = php_stream_context_get_option(context, "http", "protocol_version")) != NULL &&
		    Z_TYPE_P(tmp) == IS_DOUBLE &&
		    Z_DVAL_P(tmp) == 1.0) {
			http_1_1 = 0;
		} else {
			http_1_1 = 1;
		}

		smart_str_append_const(&soap_headers, "POST ");
		if (use_proxy && !use_ssl) {
			smart_str_appends(&soap_headers, phpurl->scheme);
			smart_str_append_const(&soap_headers, "://");
			smart_str_appends(&soap_headers, phpurl->host);
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (phpurl->path) {
			smart_str_appends(&soap_headers, phpurl->path);
		} else {
			smart_str_appendc(&soap_headers, '/');
		}
		if (phpurl->query) {
			smart_str_appendc(&soap_headers, '?');
			smart_str_appends(&soap_headers, phpurl->query);
		}
		if (phpurl->fragment) {
			smart_str_appendc(&soap_headers, '#');
			smart_str_appends(&soap_headers, phpurl->fragment);
		}
		if (http_1_1) {
			smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		} else {
			smart_str_append_const(&soap_headers, " HTTP/1.0\r\n");
		}
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != (use_ssl?443:80)) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (!http_1_1 ||
			((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_keep_alive", sizeof("_keep_alive")-1)) != NULL &&
			 (Z_TYPE_P(tmp) == IS_FALSE || (Z_TYPE_P(tmp) == IS_LONG && Z_LVAL_P(tmp) == 0)))) {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: close\r\n");
		} else {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: Keep-Alive\r\n");
		}
		if ((tmp = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_user_agent", sizeof("_user_agent")-1)) != NULL &&
		    Z_TYPE_P(tmp) == IS_STRING) {
			if (Z_STRLEN_P(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (context &&
		           (tmp = php_stream_context_get_option(context, "http", "user_agent")) != NULL &&
		           Z_TYPE_P(tmp) == IS_STRING) {
			if (Z_STRLEN_P(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (FG(user_agent)) {
			smart_str_append_const(&soap_headers, "User-Agent: ");
			smart_str_appends(&soap_headers, FG(user_agent));
			smart_str_append_const(&soap_headers, "\r\n");
		} else {
			smart_str_append_const(&soap_headers, "User-Agent: PHP-SOAP/"PHP_VERSION"\r\n");
		}

		smart_str_append_smart_str(&soap_headers, &soap_headers_z);

		if (soap_version == SOAP_1_2) {
			smart_str_append_const(&soap_headers,"Content-Type: application/soap+xml; charset=utf-8");
			if (soapaction) {
				smart_str_append_const(&soap_headers,"; action=\"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers,"\"");
			}
			smart_str_append_const(&soap_headers,"\r\n");
		} else {
			smart_str_append_const(&soap_headers,"Content-Type: text/xml; charset=utf-8\r\n");
			if (soapaction) {
				smart_str_append_const(&soap_headers, "SOAPAction: \"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers, "\"\r\n");
			}
		}
		smart_str_append_const(&soap_headers,"Content-Length: ");
		smart_str_append_long(&soap_headers, request->len);
		smart_str_append_const(&soap_headers, "\r\n");

		/* HTTP Authentication */
		if ((login = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login")-1)) != NULL &&
		    Z_TYPE_P(login) == IS_STRING) {
			zval *digest;

			has_authorization = 1;
			if ((digest = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest")-1)) != NULL) {
				if (Z_TYPE_P(digest) == IS_ARRAY) {
					char          HA1[33], HA2[33], response[33], cnonce[33], nc[9];
					PHP_MD5_CTX   md5ctx;
					unsigned char hash[16];

					PHP_MD5Init(&md5ctx);
					snprintf(cnonce, sizeof(cnonce), ZEND_LONG_FMT, php_rand());
					PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, strlen(cnonce));
					PHP_MD5Final(hash, &md5ctx);
					make_digest(cnonce, hash);

					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nc", sizeof("nc")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_LONG) {
						Z_LVAL_P(tmp)++;
						snprintf(nc, sizeof(nc), "%08ld", Z_LVAL_P(tmp));
					} else {
						add_assoc_long(digest, "nc", 1);
						strcpy(nc, "00000001");
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(login), Z_STRLEN_P(login));
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "realm", sizeof("realm")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if ((password = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password")-1)) != NULL &&
					    Z_TYPE_P(password) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(password), Z_STRLEN_P(password));
					}
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA1, hash);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "algorithm", sizeof("algorithm")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING &&
					    Z_STRLEN_P(tmp) == sizeof("md5-sess")-1 &&
					    stricmp(Z_STRVAL_P(tmp), "md5-sess") == 0) {
						PHP_MD5Init(&md5ctx);
						PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
						    Z_TYPE_P(tmp) == IS_STRING) {
							PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
						}
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
						PHP_MD5Final(hash, &md5ctx);
						make_digest(HA1, hash);
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)"POST:", sizeof("POST:")-1);
					if (phpurl->path) {
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->path, strlen(phpurl->path));
					} else {
						PHP_MD5Update(&md5ctx, (unsigned char*)"/", 1);
					}
					if (phpurl->query) {
						PHP_MD5Update(&md5ctx, (unsigned char*)"?", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->query, strlen(phpurl->query));
					}

					/* TODO: Support for qop="auth-int" */
/*
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING &&
					    Z_STRLEN_PP(tmp) == sizeof("auth-int")-1 &&
					    stricmp(Z_STRVAL_PP(tmp), "auth-int") == 0) {
						PHP_MD5Update(&md5ctx, ":", 1);
						PHP_MD5Update(&md5ctx, HEntity, HASHHEXLEN);
					}
*/
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA2, hash);

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "qop", sizeof("qop")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)nc, 8);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						/* TODO: Support for qop="auth-int" */
						PHP_MD5Update(&md5ctx, (unsigned char*)"auth", sizeof("auth")-1);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)HA2, 32);
					PHP_MD5Final(hash, &md5ctx);
					make_digest(response, hash);

					smart_str_append_const(&soap_headers, "Authorization: Digest username=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_P(login), Z_STRLEN_P(login));
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "realm", sizeof("realm")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", realm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", nonce=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					smart_str_append_const(&soap_headers, "\", uri=\"");
					if (phpurl->path) {
						smart_str_appends(&soap_headers, phpurl->path);
					} else {
						smart_str_appendc(&soap_headers, '/');
					}
					if (phpurl->query) {
						smart_str_appendc(&soap_headers, '?');
						smart_str_appends(&soap_headers, phpurl->query);
					}
					if (phpurl->fragment) {
						smart_str_appendc(&soap_headers, '#');
						smart_str_appends(&soap_headers, phpurl->fragment);
					}
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "qop", sizeof("qop")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
					/* TODO: Support for qop="auth-int" */
						smart_str_append_const(&soap_headers, "\", qop=\"auth");
						smart_str_append_const(&soap_headers, "\", nc=\"");
						smart_str_appendl(&soap_headers, nc, 8);
						smart_str_append_const(&soap_headers, "\", cnonce=\"");
						smart_str_appendl(&soap_headers, cnonce, 8);
					}
					smart_str_append_const(&soap_headers, "\", response=\"");
					smart_str_appendl(&soap_headers, response, 32);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "opaque", sizeof("opaque")-1)) != NULL &&
					    Z_TYPE_P(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", opaque=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "algorithm", sizeof("algorithm")-1)) != NULL &&
						Z_TYPE_P(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", algorithm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					smart_str_append_const(&soap_headers, "\"\r\n");
				}
			} else {
				zend_string *buf;

				smart_str auth = {0};
				smart_str_appendl(&auth, Z_STRVAL_P(login), Z_STRLEN_P(login));
				smart_str_appendc(&auth, ':');
				if ((password = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password")-1)) != NULL &&
				    Z_TYPE_P(password) == IS_STRING) {
					smart_str_appendl(&auth, Z_STRVAL_P(password), Z_STRLEN_P(password));
				}
				smart_str_0(&auth);
				buf = php_base64_encode((unsigned char*)ZSTR_VAL(auth.s), ZSTR_LEN(auth.s));
				smart_str_append_const(&soap_headers, "Authorization: Basic ");
				smart_str_appendl(&soap_headers, (char*)ZSTR_VAL(buf), ZSTR_LEN(buf));
				smart_str_append_const(&soap_headers, "\r\n");
				zend_string_release(buf);
				smart_str_free(&auth);
			}
		}

		/* Proxy HTTP Authentication */
		if (use_proxy && !use_ssl) {
			has_proxy_authorization = proxy_authentication(this_ptr, &soap_headers);
		}

		/* Send cookies along with request */
		if ((cookies = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies")-1)) != NULL &&
		    Z_TYPE_P(cookies) == IS_ARRAY) {
			zval *data;
			zend_string *key;
			int i, n;

			has_cookies = 1;
			n = zend_hash_num_elements(Z_ARRVAL_P(cookies));
			if (n > 0) {
				zend_hash_internal_pointer_reset(Z_ARRVAL_P(cookies));
				smart_str_append_const(&soap_headers, "Cookie: ");
				for (i = 0; i < n; i++) {
					zend_ulong numindx;
					int res = zend_hash_get_current_key(Z_ARRVAL_P(cookies), &key, &numindx);
					data = zend_hash_get_current_data(Z_ARRVAL_P(cookies));
					
					if (res == HASH_KEY_IS_STRING && Z_TYPE_P(data) == IS_ARRAY) {
					  zval *value;

						if ((value = zend_hash_index_find(Z_ARRVAL_P(data), 0)) != NULL &&
						    Z_TYPE_P(value) == IS_STRING) {
						  zval *tmp;
						  if (((tmp = zend_hash_index_find(Z_ARRVAL_P(data), 1)) == NULL ||
							   Z_TYPE_P(tmp) != IS_STRING ||
						       strncmp(phpurl->path?phpurl->path:"/",Z_STRVAL_P(tmp),Z_STRLEN_P(tmp)) == 0) &&
						      ((tmp = zend_hash_index_find(Z_ARRVAL_P(data), 2)) == NULL ||
							   Z_TYPE_P(tmp) != IS_STRING ||
						       in_domain(phpurl->host,Z_STRVAL_P(tmp))) &&
						      (use_ssl || (tmp = zend_hash_index_find(Z_ARRVAL_P(data), 3)) == NULL)) {
								smart_str_append(&soap_headers, key);
								smart_str_appendc(&soap_headers, '=');
								smart_str_append(&soap_headers, Z_STR_P(value));
								smart_str_appendc(&soap_headers, ';');
							}
						}
					}
					zend_hash_move_forward(Z_ARRVAL_P(cookies));
				}
				smart_str_append_const(&soap_headers, "\r\n");
			}
		}

		http_context_headers(context, has_authorization, has_proxy_authorization, has_cookies, &soap_headers);

		smart_str_append_const(&soap_headers, "\r\n");
		smart_str_0(&soap_headers);
		if ((trace = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace")-1)) != NULL &&
		    (Z_TYPE_P(trace) == IS_TRUE || (Z_TYPE_P(trace) == IS_LONG && Z_LVAL_P(trace) != 0))) {
			add_property_stringl(this_ptr, "__last_request_headers", ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s));
		}
		smart_str_appendl(&soap_headers, request->val, request->len);
		smart_str_0(&soap_headers);

		err = php_stream_write(stream, ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s));
		if (err != ZSTR_LEN(soap_headers.s)) {
			if (request != buf) {
				zend_string_release(request);
			}
			php_stream_close(stream);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl")-1);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
			add_soap_fault(this_ptr, "HTTP", "Failed Sending HTTP SOAP request", NULL, NULL);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
		smart_str_free(&soap_headers);
	} else {
		add_soap_fault(this_ptr, "HTTP", "Failed to create stream??", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (!return_value) {
		php_stream_close(stream);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
		smart_str_free(&soap_headers_z);
		return TRUE;
	}

	do {
		http_headers = get_http_headers(stream);
		if (!http_headers) {
			if (request != buf) {
				zend_string_release(request);
			}
			php_stream_close(stream);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
			zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
			add_soap_fault(this_ptr, "HTTP", "Error Fetching http headers", NULL, NULL);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}

		if ((trace = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace")-1)) != NULL &&
		    (Z_TYPE_P(trace) == IS_TRUE || (Z_TYPE_P(trace) == IS_LONG && Z_LVAL_P(trace) != 0))) {
			add_property_str(this_ptr, "__last_response_headers", zend_string_copy(http_headers));
		}

		/* Check to see what HTTP status was sent */
		http_1_1 = 0;
		http_status = 0;
		http_version = get_http_header_value(ZSTR_VAL(http_headers), "HTTP/");
		if (http_version) {
			char *tmp;

			if (!strncmp(http_version,"1.1", 3)) {
				http_1_1 = 1;
			}

			tmp = strstr(http_version," ");
			if (tmp != NULL) {
				tmp++;
				http_status = atoi(tmp);
			}
			tmp = strstr(tmp," ");
			if (tmp != NULL) {
				tmp++;
				if (http_msg) {
					efree(http_msg);
				}
				http_msg = estrdup(tmp);
			}
			efree(http_version);

			/* Try and get headers again */
			if (http_status == 100) {
				zend_string_release(http_headers);
			}
		}
	} while (http_status == 100);

	/* Grab and send back every cookie */

	/* Not going to worry about Path: because
	   we shouldn't be changing urls so path dont
	   matter too much
	*/
	cookie_itt = strstr(ZSTR_VAL(http_headers), "Set-Cookie: ");
	while (cookie_itt) {
		char *cookie;
		char *eqpos, *sempos;
		zval *cookies;

		if ((cookies = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies")-1)) == NULL ||
		    Z_TYPE_P(cookies) != IS_ARRAY) {
			zval tmp_cookies;
			array_init(&tmp_cookies);
			cookies = zend_hash_str_update(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies")-1, &tmp_cookies);
		}

		cookie = get_http_header_value(cookie_itt,"Set-Cookie: ");

		eqpos = strstr(cookie, "=");
		sempos = strstr(cookie, ";");
		if (eqpos != NULL && (sempos == NULL || sempos > eqpos)) {
			smart_str name = {0};
			int cookie_len;
			zval zcookie;

			if (sempos != NULL) {
				cookie_len = sempos-(eqpos+1);
			} else {
				cookie_len = strlen(cookie)-(eqpos-cookie)-1;
			}

			smart_str_appendl(&name, cookie, eqpos - cookie);
			smart_str_0(&name);

			array_init(&zcookie);
			add_index_stringl(&zcookie, 0, eqpos + 1, cookie_len);

			if (sempos != NULL) {
				char *options = cookie + cookie_len+1;
				while (*options) {
					while (*options == ' ') {options++;}
					sempos = strstr(options, ";");
					if (strstr(options,"path=") == options) {
						eqpos = options + sizeof("path=")-1;
						add_index_stringl(&zcookie, 1, eqpos, sempos?(sempos-eqpos):strlen(eqpos));
					} else if (strstr(options,"domain=") == options) {
						eqpos = options + sizeof("domain=")-1;
						add_index_stringl(&zcookie, 2, eqpos, sempos?(sempos-eqpos):strlen(eqpos));
					} else if (strstr(options,"secure") == options) {
						add_index_bool(&zcookie, 3, 1);
					}
					if (sempos != NULL) {
						options = sempos+1;
					} else {
					  break;
					}
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL(zcookie), 1)) {
				char *t = phpurl->path?phpurl->path:"/";
				char *c = strrchr(t, '/');
				if (c) {
					add_index_stringl(&zcookie, 1, t, c-t);
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL(zcookie), 2)) {
				add_index_string(&zcookie, 2, phpurl->host);
			}

			zend_symtable_update(Z_ARRVAL_P(cookies), name.s, &zcookie);
			smart_str_free(&name);
		}

		cookie_itt = strstr(cookie_itt + sizeof("Set-Cookie: "), "Set-Cookie: ");
		efree(cookie);
	}

	/* See if the server requested a close */
	if (http_1_1) {
		http_close = FALSE;
		if (use_proxy && !use_ssl) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "close", sizeof("close")-1) == 0) {
					http_close = TRUE;
				}
				efree(connection);
			}
		}
		if (http_close == FALSE) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Connection: ");
			if (connection) {
				if (strncasecmp(connection, "close", sizeof("close")-1) == 0) {
					http_close = TRUE;
				}
				efree(connection);
			}
		}
	} else {
		http_close = TRUE;
		if (use_proxy && !use_ssl) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
		if (http_close == TRUE) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
	}


	http_body = get_http_body(stream, http_close, ZSTR_VAL(http_headers));
	if (!http_body) {
		if (request != buf) {
			zend_string_release(request);
		}
		php_stream_close(stream);
		zend_string_release(http_headers);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
		add_soap_fault(this_ptr, "HTTP", "Error Fetching http body, No Content-Length, connection closed or chunked data", NULL, NULL);
		if (http_msg) {
			efree(http_msg);
		}
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (request != buf) {
		zend_string_release(request);
	}

	if (http_close) {
		php_stream_close(stream);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket")-1);
		zend_hash_str_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy")-1);
		stream = NULL;
	}

	/* Process HTTP status codes */
	if (http_status >= 300 && http_status < 400) {
		char *loc;

		if ((loc = get_http_header_value(ZSTR_VAL(http_headers), "Location: ")) != NULL) {
			php_url *new_url  = php_url_parse(loc);

			if (new_url != NULL) {
				zend_string_release(http_headers);
				zend_string_release(http_body);
				efree(loc);
				if (new_url->scheme == NULL && new_url->path != NULL) {
					new_url->scheme = phpurl->scheme ? estrdup(phpurl->scheme) : NULL;
					new_url->host = phpurl->host ? estrdup(phpurl->host) : NULL;
					new_url->port = phpurl->port;
					if (new_url->path && new_url->path[0] != '/') {
						if (phpurl->path) {
							char *t = phpurl->path;
							char *p = strrchr(t, '/');
							if (p) {
								char *s = emalloc((p - t) + strlen(new_url->path) + 2);
								strncpy(s, t, (p - t) + 1);
								s[(p - t) + 1] = 0;
								strcat(s, new_url->path);
								efree(new_url->path);
								new_url->path = s;
							}
						} else {
							char *s = emalloc(strlen(new_url->path) + 2);
							s[0] = '/'; s[1] = 0;
							strcat(s, new_url->path);
							efree(new_url->path);
							new_url->path = s;
						}
					}
				}
				phpurl = new_url;

				if (--redirect_max < 1) {
					add_soap_fault(this_ptr, "HTTP", "Redirection limit reached, aborting", NULL, NULL);
					smart_str_free(&soap_headers_z);
					return FALSE;
				}

				goto try_again;
			}
		}
	} else if (http_status == 401) {
		/* Digest authentication */
		zval *digest, *login, *password;
		char *auth = get_http_header_value(ZSTR_VAL(http_headers), "WWW-Authenticate: ");

		if (auth &&
				strstr(auth, "Digest") == auth &&
		    ((digest = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest")-1)) == NULL ||
		     Z_TYPE_P(digest) != IS_ARRAY) &&
		    (login = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login")-1)) != NULL &&
		    Z_TYPE_P(login) == IS_STRING &&
		    (password = zend_hash_str_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password")-1)) != NULL &&
		    Z_TYPE_P(password) == IS_STRING) {
			char *s;
			zval digest;

			ZVAL_UNDEF(&digest);
			s = auth + sizeof("Digest")-1;
			while (*s != '\0') {
				char *name, *val;
				while (*s == ' ') ++s;
				name = s;
				while (*s != '\0' && *s != '=') ++s;
				if (*s == '=') {
					*s = '\0';
					++s;
					if (*s == '"') {
						++s;
						val = s;
						while (*s != '\0' && *s != '"') ++s;
					} else {
						val = s;
						while (*s != '\0' && *s != ' ' && *s != ',') ++s;
					}
					if (*s != '\0') {
						if (*s != ',') {
							*s = '\0';
							++s;
							while (*s != '\0' && *s != ',') ++s;
							if (*s != '\0') ++s;
						} else {
							*s = '\0';
							++s;
						}
					}
					if (Z_TYPE(digest) == IS_UNDEF) {
						array_init(&digest);
					}
					add_assoc_string(&digest, name, val);
				}
			}

			if (Z_TYPE(digest) != IS_UNDEF) {
				php_url *new_url  = emalloc(sizeof(php_url));

				Z_DELREF(digest);
				add_property_zval_ex(this_ptr, "_digest", sizeof("_digest")-1, &digest);

				*new_url = *phpurl;
				if (phpurl->scheme) phpurl->scheme = estrdup(phpurl->scheme);
				if (phpurl->user) phpurl->user = estrdup(phpurl->user);
				if (phpurl->pass) phpurl->pass = estrdup(phpurl->pass);
				if (phpurl->host) phpurl->host = estrdup(phpurl->host);
				if (phpurl->path) phpurl->path = estrdup(phpurl->path);
				if (phpurl->query) phpurl->query = estrdup(phpurl->query);
				if (phpurl->fragment) phpurl->fragment = estrdup(phpurl->fragment);
				phpurl = new_url;

				efree(auth);
				zend_string_release(http_headers);
				zend_string_release(http_body);

				goto try_again;
			}
		}
		if (auth) efree(auth);
	}
	smart_str_free(&soap_headers_z);

	/* Check and see if the server even sent a xml document */
	content_type = get_http_header_value(ZSTR_VAL(http_headers), "Content-Type: ");
	if (content_type) {
		char *pos = NULL;
		int cmplen;
		pos = strstr(content_type,";");
		if (pos != NULL) {
			cmplen = pos - content_type;
		} else {
			cmplen = strlen(content_type);
		}
		if (strncmp(content_type, "text/xml", cmplen) == 0 ||
		    strncmp(content_type, "application/soap+xml", cmplen) == 0) {
			content_type_xml = 1;
/*
			if (strncmp(http_body, "<?xml", 5)) {
				zval *err;
				MAKE_STD_ZVAL(err);
				ZVAL_STRINGL(err, http_body, http_body_size, 1);
				add_soap_fault(this_ptr, "HTTP", "Didn't receive an xml document", NULL, err);
				efree(content_type);
				zend_string_release(http_headers);
				efree(http_body);
				return FALSE;
			}
*/
		}
		efree(content_type);
	}

	/* Decompress response */
	content_encoding = get_http_header_value(ZSTR_VAL(http_headers), "Content-Encoding: ");
	if (content_encoding) {
		zval func;
		zval retval;
		zval params[1];

		if ((strcmp(content_encoding,"gzip") == 0 ||
		     strcmp(content_encoding,"x-gzip") == 0) &&
		     zend_hash_str_exists(EG(function_table), "gzinflate", sizeof("gzinflate")-1)) {
			ZVAL_STRING(&func, "gzinflate");
			ZVAL_STRINGL(&params[0], http_body->val+10, http_body->len-10);
		} else if (strcmp(content_encoding,"deflate") == 0 &&
		           zend_hash_str_exists(EG(function_table), "gzuncompress", sizeof("gzuncompress")-1)) {
			ZVAL_STRING(&func, "gzuncompress");
			ZVAL_STR_COPY(&params[0], http_body);
		} else {
			efree(content_encoding);
			zend_string_release(http_headers);
			zend_string_release(http_body);
			if (http_msg) {
				efree(http_msg);
			}
			add_soap_fault(this_ptr, "HTTP", "Unknown Content-Encoding", NULL, NULL);
			return FALSE;
		}
		if (call_user_function(CG(function_table), (zval*)NULL, &func, &retval, 1, params) == SUCCESS &&
		    Z_TYPE(retval) == IS_STRING) {
			zval_ptr_dtor(&params[0]);
			zval_ptr_dtor(&func);
			zend_string_release(http_body);
			ZVAL_COPY_VALUE(return_value, &retval);
		} else {
			zval_ptr_dtor(&params[0]);
			zval_ptr_dtor(&func);
			efree(content_encoding);
			zend_string_release(http_headers);
			zend_string_release(http_body);
			add_soap_fault(this_ptr, "HTTP", "Can't uncompress compressed response", NULL, NULL);
			if (http_msg) {
				efree(http_msg);
			}
			return FALSE;
		}
		efree(content_encoding);
	} else {
		ZVAL_STR(return_value, http_body);
	}

	zend_string_release(http_headers);

	if (http_status >= 400) {
		int error = 0;

		if (Z_STRLEN_P(return_value) == 0) {
			error = 1;
		} else if (Z_STRLEN_P(return_value) > 0) {
			if (!content_type_xml) {
				char *s = Z_STRVAL_P(return_value);

				while (*s != '\0' && *s < ' ') {
					s++;
				}
				if (strncmp(s, "<?xml", 5)) {
					error = 1;
				}
			}
		}

		if (error) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			add_soap_fault(this_ptr, "HTTP", http_msg, NULL, NULL);
			efree(http_msg);
			return FALSE;
		}
	}

	if (http_msg) {
		efree(http_msg);
	}

	return TRUE;
}