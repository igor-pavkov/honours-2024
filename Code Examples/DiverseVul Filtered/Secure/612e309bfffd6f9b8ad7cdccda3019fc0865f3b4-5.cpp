static apr_byte_t oidc_refresh_claims_from_userinfo_endpoint(request_rec *r,
		oidc_cfg *cfg, oidc_session_t *session) {

	oidc_provider_t *provider = NULL;
	const char *claims = NULL;
	char *access_token = NULL;

	/* get the current provider info */
	if (oidc_get_provider_from_session(r, cfg, session, &provider) == FALSE)
		return FALSE;

	/* see if we can do anything here, i.e. we have a userinfo endpoint and a refresh interval is configured */
	apr_time_t interval = apr_time_from_sec(
			provider->userinfo_refresh_interval);

	oidc_debug(r, "userinfo_endpoint=%s, interval=%d",
			provider->userinfo_endpoint_url,
			provider->userinfo_refresh_interval);

	if ((provider->userinfo_endpoint_url != NULL) && (interval > 0)) {

		/* get the last refresh timestamp from the session info */
		apr_time_t last_refresh = 0;
		const char *s_last_refresh = NULL;
		oidc_session_get(r, session, OIDC_USERINFO_LAST_REFRESH_SESSION_KEY,
				&s_last_refresh);
		if (s_last_refresh != NULL) {
			sscanf(s_last_refresh, "%" APR_TIME_T_FMT, &last_refresh);
		}

		oidc_debug(r, "refresh needed in: %" APR_TIME_T_FMT " seconds",
				apr_time_sec(last_refresh + interval - apr_time_now()));

		/* see if we need to refresh again */
		if (last_refresh + interval < apr_time_now()) {

			/* get the current access token */
			oidc_session_get(r, session, OIDC_ACCESSTOKEN_SESSION_KEY,
					(const char **) &access_token);

			/* retrieve the current claims */
			claims = oidc_retrieve_claims_from_userinfo_endpoint(r, cfg,
					provider, access_token, session, NULL);

			/* store claims resolved from userinfo endpoint */
			oidc_store_userinfo_claims(r, session, provider, claims);

			/* indicated something changed */
			return TRUE;
		}
	}
	return FALSE;
}