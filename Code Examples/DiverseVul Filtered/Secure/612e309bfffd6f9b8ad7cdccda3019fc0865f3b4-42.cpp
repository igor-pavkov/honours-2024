static int oidc_handle_refresh_token_request(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {

	char *return_to = NULL;
	char *r_access_token = NULL;
	char *error_code = NULL;

	/* get the command passed to the session management handler */
	oidc_util_get_request_parameter(r, "refresh", &return_to);
	oidc_util_get_request_parameter(r, "access_token", &r_access_token);

	/* check the input parameters */
	if (return_to == NULL) {
		oidc_error(r,
				"refresh token request handler called with no URL to return to");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (r_access_token == NULL) {
		oidc_error(r,
				"refresh token request handler called with no access_token parameter");
		error_code = "no_access_token";
		goto end;
	}

	char *s_access_token = NULL;
	oidc_session_get(r, session, OIDC_ACCESSTOKEN_SESSION_KEY,
			(const char **) &s_access_token);
	if (s_access_token == NULL) {
		oidc_error(r,
				"no existing access_token found in the session, nothing to refresh");
		error_code = "no_access_token_exists";
		goto end;
	}

	/* compare the access_token parameter used for XSRF protection */
	if (apr_strnatcmp(s_access_token, r_access_token) != 0) {
		oidc_error(r,
				"access_token passed in refresh request does not match the one stored in the session");
		error_code = "no_access_token_match";
		goto end;
	}

	/* get a handle to the provider configuration */
	oidc_provider_t *provider = NULL;
	if (oidc_get_provider_from_session(r, c, session, &provider) == FALSE) {
		error_code = "session_corruption";
		goto end;
	}

	/* execute the actual refresh grant */
	if (oidc_refresh_access_token(r, c, session, provider, NULL) == FALSE) {
		oidc_error(r, "access_token could not be refreshed");
		error_code = "refresh_failed";
		goto end;
	}

	/* store the session */
	if (oidc_session_save(r, session) == FALSE) {
		error_code = "session_corruption";
		goto end;
	}

end:

	/* pass optional error message to the return URL */
	if (error_code != NULL)
		return_to = apr_psprintf(r->pool, "%s%serror_code=%s", return_to,
				strchr(return_to, '?') ? "&" : "?",
						oidc_util_escape_string(r, error_code));

	/* add the redirect location header */
	apr_table_add(r->headers_out, "Location", return_to);

	return HTTP_MOVED_TEMPORARILY;
}