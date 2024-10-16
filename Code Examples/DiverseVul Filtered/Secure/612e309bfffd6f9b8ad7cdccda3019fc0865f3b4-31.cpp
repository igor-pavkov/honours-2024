static void oidc_strip_cookies(request_rec *r) {

	char *cookie, *ctx, *result = NULL;
	const char *name = NULL;
	int i;

	apr_array_header_t *strip = oidc_dir_cfg_strip_cookies(r);

	char *cookies = apr_pstrdup(r->pool,
			(char *) apr_table_get(r->headers_in, "Cookie"));

	if ((cookies != NULL) && (strip != NULL)) {

		oidc_debug(r,
				"looking for the following cookies to strip from cookie header: %s",
				apr_array_pstrcat(r->pool, strip, ','));

		cookie = apr_strtok(cookies, ";", &ctx);

		do {
			while (cookie != NULL && *cookie == ' ')
				cookie++;

			for (i = 0; i < strip->nelts; i++) {
				name = ((const char**) strip->elts)[i];
				if ((strncmp(cookie, name, strlen(name)) == 0)
						&& (cookie[strlen(name)] == '=')) {
					oidc_debug(r, "stripping: %s", name);
					break;
				}
			}

			if (i == strip->nelts) {
				result =
						result ?
								apr_psprintf(r->pool, "%s;%s", result, cookie) :
								cookie;
			}

			cookie = apr_strtok(NULL, ";", &ctx);
		} while (cookie != NULL);

		if (result != NULL) {
			oidc_debug(r, "set cookie to backend to: %s",
					result ?
							apr_psprintf(r->pool, "\"%s\"", result) : "<null>");
			apr_table_set(r->headers_in, "Cookie", result);
		} else {
			oidc_debug(r, "unsetting all cookies to backend");
			apr_table_unset(r->headers_in, "Cookie");
		}

	}
}