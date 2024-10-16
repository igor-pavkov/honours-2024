static apr_byte_t oidc_save_in_session(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, oidc_provider_t *provider,
		const char *remoteUser, const char *id_token, oidc_jwt_t *id_token_jwt,
		const char *claims, const char *access_token, const int expires_in,
		const char *refresh_token, const char *session_state, const char *state,
		const char *original_url) {

	/* store the user in the session */
	session->remote_user = remoteUser;

	/* set the session expiry to the inactivity timeout */
	session->expiry =
			apr_time_now() + apr_time_from_sec(c->session_inactivity_timeout);

	/* store the claims payload in the id_token for later reference */
	oidc_session_set(r, session, OIDC_IDTOKEN_CLAIMS_SESSION_KEY,
			id_token_jwt->payload.value.str);

	if (c->session_type != OIDC_SESSION_TYPE_CLIENT_COOKIE) {
		/* store the compact serialized representation of the id_token for later reference  */
		oidc_session_set(r, session, OIDC_IDTOKEN_SESSION_KEY, id_token);
	}

	/* store the issuer in the session (at least needed for session mgmt and token refresh */
	oidc_session_set(r, session, OIDC_ISSUER_SESSION_KEY, provider->issuer);

	/* store the state and original URL in the session for handling browser-back more elegantly */
	oidc_session_set(r, session, OIDC_REQUEST_STATE_SESSION_KEY, state);
	oidc_session_set(r, session, OIDC_REQUEST_ORIGINAL_URL, original_url);

	if ((session_state != NULL) && (provider->check_session_iframe != NULL)) {
		/* store the session state and required parameters session management  */
		oidc_session_set(r, session, OIDC_SESSION_STATE_SESSION_KEY,
				session_state);
		oidc_session_set(r, session, OIDC_CHECK_IFRAME_SESSION_KEY,
				provider->check_session_iframe);
		oidc_session_set(r, session, OIDC_CLIENTID_SESSION_KEY,
				provider->client_id);
		oidc_debug(r,
				"session management enabled: stored session_state (%s), check_session_iframe (%s) and client_id (%s) in the session",
				session_state, provider->check_session_iframe,
				provider->client_id);
	} else {
		oidc_debug(r,
				"session management disabled: session_state (%s) and/or check_session_iframe (%s) is not provided",
				session_state, provider->check_session_iframe);
	}

	if (provider->end_session_endpoint != NULL)
		oidc_session_set(r, session, OIDC_LOGOUT_ENDPOINT_SESSION_KEY,
				provider->end_session_endpoint);

	/* store claims resolved from userinfo endpoint */
	oidc_store_userinfo_claims(r, session, provider, claims);

	/* see if we have an access_token */
	if (access_token != NULL) {
		/* store the access_token in the session context */
		oidc_session_set(r, session, OIDC_ACCESSTOKEN_SESSION_KEY,
				access_token);
		/* store the associated expires_in value */
		oidc_store_access_token_expiry(r, session, expires_in);
	}

	/* see if we have a refresh_token */
	if (refresh_token != NULL) {
		/* store the refresh_token in the session context */
		oidc_session_set(r, session, OIDC_REFRESHTOKEN_SESSION_KEY,
				refresh_token);
	}

	/* store max session duration in the session as a hard cut-off expiry timestamp */
	apr_time_t session_expires =
			(provider->session_max_duration == 0) ?
					apr_time_from_sec(id_token_jwt->payload.exp) :
					(apr_time_now()
							+ apr_time_from_sec(provider->session_max_duration));
	oidc_session_set(r, session, OIDC_SESSION_EXPIRES_SESSION_KEY,
			apr_psprintf(r->pool, "%" APR_TIME_T_FMT, session_expires));

	/* log message about max session duration */
	oidc_log_session_expires(r, session_expires);

	/* store the domain for which this session is valid */
	oidc_session_set(r, session, OIDC_COOKIE_DOMAIN_SESSION_KEY,
			c->cookie_domain ? c->cookie_domain : oidc_get_current_url_host(r));

	/* store the session */
	return oidc_session_save(r, session);
}