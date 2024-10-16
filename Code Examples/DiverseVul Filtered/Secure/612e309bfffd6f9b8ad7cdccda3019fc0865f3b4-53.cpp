static void oidc_store_access_token_expiry(request_rec *r,
		oidc_session_t *session, int expires_in) {
	if (expires_in != -1) {
		oidc_session_set(r, session, OIDC_ACCESSTOKEN_EXPIRES_SESSION_KEY,
				apr_psprintf(r->pool, "%" APR_TIME_T_FMT,
						apr_time_sec(apr_time_now()) + expires_in));
	}
}