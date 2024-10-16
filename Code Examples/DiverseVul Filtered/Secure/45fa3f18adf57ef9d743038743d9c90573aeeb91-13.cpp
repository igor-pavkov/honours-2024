static void parse_url(const char *src_url, struct host_info *h)
{
	char *url, *p, *sp;

	free(h->allocated);
	h->allocated = url = xstrdup(src_url);

	h->protocol = P_FTP;
	p = strstr(url, "://");
	if (p) {
		*p = '\0';
		h->host = p + 3;
		if (strcmp(url, P_FTP) == 0) {
			h->port = bb_lookup_std_port(P_FTP, "tcp", 21);
		} else
#if SSL_SUPPORTED
# if ENABLE_FEATURE_WGET_HTTPS
		if (strcmp(url, P_FTPS) == 0) {
			h->port = bb_lookup_std_port(P_FTPS, "tcp", 990);
			h->protocol = P_FTPS;
		} else
# endif
		if (strcmp(url, P_HTTPS) == 0) {
			h->port = bb_lookup_std_port(P_HTTPS, "tcp", 443);
			h->protocol = P_HTTPS;
		} else
#endif
		if (strcmp(url, P_HTTP) == 0) {
 http:
			h->port = bb_lookup_std_port(P_HTTP, "tcp", 80);
			h->protocol = P_HTTP;
		} else {
			*p = ':';
			bb_error_msg_and_die("not an http or ftp url: %s", url);
		}
	} else {
		// GNU wget is user-friendly and falls back to http://
		h->host = url;
		goto http;
	}

	// FYI:
	// "Real" wget 'http://busybox.net?var=a/b' sends this request:
	//   'GET /?var=a/b HTTP/1.0'
	//   and saves 'index.html?var=a%2Fb' (we save 'b')
	// wget 'http://busybox.net?login=john@doe':
	//   request: 'GET /?login=john@doe HTTP/1.0'
	//   saves: 'index.html?login=john@doe' (we save 'login=john@doe')
	// wget 'http://busybox.net#test/test':
	//   request: 'GET / HTTP/1.0'
	//   saves: 'index.html' (we save 'test')
	//
	// We also don't add unique .N suffix if file exists...
	sp = strchr(h->host, '/');
	p = strchr(h->host, '?'); if (!sp || (p && sp > p)) sp = p;
	p = strchr(h->host, '#'); if (!sp || (p && sp > p)) sp = p;
	if (!sp) {
		h->path = "";
	} else if (*sp == '/') {
		*sp = '\0';
		h->path = sp + 1;
	} else {
		// sp points to '#' or '?'
		// Note:
		// http://busybox.net?login=john@doe is a valid URL
		// (without '/' between ".net" and "?"),
		// can't store NUL at sp[-1] - this destroys hostname.
		*sp++ = '\0';
		h->path = sp;
	}

	sp = strrchr(h->host, '@');
	if (sp != NULL) {
		// URL-decode "user:password" string before base64-encoding:
		// wget http://test:my%20pass@example.com should send
		// Authorization: Basic dGVzdDpteSBwYXNz
		// which decodes to "test:my pass".
		// Standard wget and curl do this too.
		*sp = '\0';
		free(h->user);
		h->user = xstrdup(percent_decode_in_place(h->host, /*strict:*/ 0));
		h->host = sp + 1;
	}
	/* else: h->user remains NULL, or as set by original request
	 * before redirect (if we are here after a redirect).
	 */
}