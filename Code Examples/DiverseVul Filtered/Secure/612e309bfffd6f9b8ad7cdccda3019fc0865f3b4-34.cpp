static apr_byte_t oidc_handle_browser_back(request_rec *r, const char *r_state,
		oidc_session_t *session) {

	/*  see if we have an existing session and browser-back was used */
	const char *s_state = NULL, *o_url = NULL;

	if (session->remote_user != NULL) {

		oidc_session_get(r, session, OIDC_REQUEST_STATE_SESSION_KEY, &s_state);
		oidc_session_get(r, session, OIDC_REQUEST_ORIGINAL_URL, &o_url);

		if ((r_state != NULL) && (s_state != NULL)
				&& (apr_strnatcmp(r_state, s_state) == 0)) {

			/* log the browser back event detection */
			oidc_warn(r,
					"browser back detected, redirecting to original URL: %s",
					o_url);

			/* go back to the URL that he originally tried to access */
			apr_table_add(r->headers_out, "Location", o_url);

			return TRUE;
		}
	}

	return FALSE;
}