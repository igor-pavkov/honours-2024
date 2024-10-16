static apr_byte_t oidc_check_cookie_domain(request_rec *r, oidc_cfg *cfg,
		oidc_session_t *session) {
	const char *c_cookie_domain =
			cfg->cookie_domain ?
					cfg->cookie_domain : oidc_get_current_url_host(r);
	const char *s_cookie_domain = NULL;
	oidc_session_get(r, session, OIDC_COOKIE_DOMAIN_SESSION_KEY,
			&s_cookie_domain);
	if ((s_cookie_domain == NULL)
			|| (apr_strnatcmp(c_cookie_domain, s_cookie_domain) != 0)) {
		oidc_warn(r,
				"aborting: detected attempt to play cookie against a different domain/host than issued for! (issued=%s, current=%s)",
				s_cookie_domain, c_cookie_domain);
		return FALSE;
	}

	return TRUE;
}