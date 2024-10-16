static void oidc_clean_expired_state_cookies(request_rec *r, oidc_cfg *c) {
	char *cookie, *tokenizerCtx;
	char *cookies = apr_pstrdup(r->pool,
			(char *) apr_table_get(r->headers_in, "Cookie"));
	if (cookies != NULL) {
		cookie = apr_strtok(cookies, ";", &tokenizerCtx);
		do {
			while (cookie != NULL && *cookie == ' ')
				cookie++;
			if (strstr(cookie, OIDCStateCookiePrefix) == cookie) {
				char *cookieName = cookie;
				while (cookie != NULL && *cookie != '=')
					cookie++;
				if (*cookie == '=') {
					*cookie = '\0';
					cookie++;
					json_t *state = oidc_get_state_from_cookie(r, c, cookie);
					if (state != NULL) {
						json_t *v = json_object_get(state, "timestamp");
						apr_time_t now = apr_time_sec(apr_time_now());
						if (now > json_integer_value(v) + c->state_timeout) {
							oidc_error(r, "state has expired");
							oidc_util_set_cookie(r, cookieName, "", 0);
						}
						json_decref(state);
					}
				}
			}
			cookie = apr_strtok(NULL, ";", &tokenizerCtx);
		} while (cookie != NULL);
	}
}