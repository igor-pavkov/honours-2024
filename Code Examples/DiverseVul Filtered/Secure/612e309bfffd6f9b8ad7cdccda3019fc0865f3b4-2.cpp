static void oidc_store_userinfo_claims(request_rec *r, oidc_session_t *session,
		oidc_provider_t *provider, const char *claims) {

	oidc_debug(r, "enter");

	/* see if we've resolved any claims */
	if (claims != NULL) {
		/*
		 * Successfully decoded a set claims from the response so we can store them
		 * (well actually the stringified representation in the response)
		 * in the session context safely now
		 */
		oidc_session_set(r, session, OIDC_CLAIMS_SESSION_KEY, claims);

	} else {
		/*
		 * clear the existing claims because we could not refresh them
		 */
		oidc_session_set(r, session, OIDC_CLAIMS_SESSION_KEY, NULL);

	}

	/* store the last refresh time if we've configured a userinfo refresh interval */
	if (provider->userinfo_refresh_interval > 0)
		oidc_session_set(r, session, OIDC_USERINFO_LAST_REFRESH_SESSION_KEY,
				apr_psprintf(r->pool, "%" APR_TIME_T_FMT, apr_time_now()));
}