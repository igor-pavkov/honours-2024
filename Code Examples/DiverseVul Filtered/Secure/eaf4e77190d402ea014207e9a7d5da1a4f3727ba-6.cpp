void http_context_headers(php_stream_context* context,
                          zend_bool has_authorization,
                          zend_bool has_proxy_authorization,
                          zend_bool has_cookies,
                          smart_str* soap_headers)
{
	zval *tmp;

	if (context &&
		(tmp = php_stream_context_get_option(context, "http", "header")) != NULL &&
		Z_TYPE_P(tmp) == IS_STRING && Z_STRLEN_P(tmp)) {
		char *s = Z_STRVAL_P(tmp);
		char *p;
		int name_len;

		while (*s) {
			/* skip leading newlines and spaces */
			while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
				s++;
			}
			/* extract header name */
			p = s;
			name_len = -1;
			while (*p) {
				if (*p == ':') {
					if (name_len < 0) name_len = p - s;
					break;
				} else if (*p == ' ' || *p == '\t') {
					if (name_len < 0) name_len = p - s;
				} else if (*p == '\r' || *p == '\n') {
					break;
				}
				p++;
			}
			if (*p == ':') {
				/* extract header value */
				while (*p && *p != '\r' && *p != '\n') {
					p++;
				}
				/* skip some predefined headers */
				if ((name_len != sizeof("host")-1 ||
				     strncasecmp(s, "host", sizeof("host")-1) != 0) &&
				    (name_len != sizeof("connection")-1 ||
				     strncasecmp(s, "connection", sizeof("connection")-1) != 0) &&
				    (name_len != sizeof("user-agent")-1 ||
				     strncasecmp(s, "user-agent", sizeof("user-agent")-1) != 0) &&
				    (name_len != sizeof("content-length")-1 ||
				     strncasecmp(s, "content-length", sizeof("content-length")-1) != 0) &&
				    (name_len != sizeof("content-type")-1 ||
				     strncasecmp(s, "content-type", sizeof("content-type")-1) != 0) &&
				    (!has_cookies ||
				     name_len != sizeof("cookie")-1 ||
				     strncasecmp(s, "cookie", sizeof("cookie")-1) != 0) &&
				    (!has_authorization ||
				     name_len != sizeof("authorization")-1 ||
				     strncasecmp(s, "authorization", sizeof("authorization")-1) != 0) &&
				    (!has_proxy_authorization ||
				     name_len != sizeof("proxy-authorization")-1 ||
				     strncasecmp(s, "proxy-authorization", sizeof("proxy-authorization")-1) != 0)) {
				    /* add header */
					smart_str_appendl(soap_headers, s, p-s);
					smart_str_append_const(soap_headers, "\r\n");
				}
			}
			s = (*p) ? (p + 1) : p;
		}
	}
}