static int oidc_handle_authorization_response(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, apr_table_t *params, const char *response_mode) {

	oidc_debug(r, "enter, response_mode=%s", response_mode);

	oidc_provider_t *provider = NULL;
	json_t *proto_state = NULL;
	oidc_jwt_t *jwt = NULL;

	/* see if this response came from a browser-back event */
	if (oidc_handle_browser_back(r, apr_table_get(params, "state"),
			session) == TRUE)
		return HTTP_MOVED_TEMPORARILY;

	/* match the returned state parameter against the state stored in the browser */
	if (oidc_authorization_response_match_state(r, c,
			apr_table_get(params, "state"), &provider, &proto_state) == FALSE) {
		if (c->default_sso_url != NULL) {
			oidc_warn(r,
					"invalid authorization response state; a default SSO URL is set, sending the user there: %s",
					c->default_sso_url);
			apr_table_add(r->headers_out, "Location", c->default_sso_url);
			return HTTP_MOVED_TEMPORARILY;
		}
		oidc_error(r,
				"invalid authorization response state and no default SSO URL is set, sending an error...");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* see if the response is an error response */
	if (apr_table_get(params, "error") != NULL)
		return oidc_authorization_response_error(r, c, proto_state,
				apr_table_get(params, "error"),
				apr_table_get(params, "error_description"));

	/* handle the code, implicit or hybrid flow */
	if (oidc_handle_flows(r, c, proto_state, provider, params, response_mode,
			&jwt) == FALSE)
		return oidc_authorization_response_error(r, c, proto_state,
				"Error in handling response type.", NULL);

	if (jwt == NULL) {
		oidc_error(r, "no id_token was provided");
		return oidc_authorization_response_error(r, c, proto_state,
				"No id_token was provided.", NULL);
	}

	int expires_in = oidc_parse_expires_in(r,
			apr_table_get(params, "expires_in"));

	/*
	 * optionally resolve additional claims against the userinfo endpoint
	 * parsed claims are not actually used here but need to be parsed anyway for error checking purposes
	 */
	const char *claims = oidc_retrieve_claims_from_userinfo_endpoint(r, c,
			provider, apr_table_get(params, "access_token"), NULL, jwt->payload.sub);

	/* restore the original protected URL that the user was trying to access */
	const char *original_url = apr_pstrdup(r->pool,
			json_string_value(json_object_get(proto_state, "original_url")));
	const char *original_method = apr_pstrdup(r->pool,
			json_string_value(json_object_get(proto_state, "original_method")));

	/* set the user */
	if (oidc_get_remote_user(r, c, provider, jwt, &r->user, claims) == TRUE) {

		/* session management: if the user in the new response is not equal to the old one, error out */
		if ((json_object_get(proto_state, "prompt") != NULL)
				&& (apr_strnatcmp(
						json_string_value(
								json_object_get(proto_state, "prompt")), "none")
						== 0)) {
			// TOOD: actually need to compare sub? (need to store it in the session separately then
			//const char *sub = NULL;
			//oidc_session_get(r, session, "sub", &sub);
			//if (apr_strnatcmp(sub, jwt->payload.sub) != 0) {
			if (apr_strnatcmp(session->remote_user, r->user) != 0) {
				oidc_warn(r,
						"user set from new id_token is different from current one");
				oidc_jwt_destroy(jwt);
				return oidc_authorization_response_error(r, c, proto_state,
						"User changed!", NULL);
			}
		}

		/* store resolved information in the session */
		if (oidc_save_in_session(r, c, session, provider, r->user,
				apr_table_get(params, "id_token"), jwt, claims,
				apr_table_get(params, "access_token"), expires_in,
				apr_table_get(params, "refresh_token"),
				apr_table_get(params, "session_state"),
				apr_table_get(params, "state"), original_url) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;

	} else {
		oidc_error(r, "remote user could not be set");
		return oidc_authorization_response_error(r, c, proto_state,
				"Remote user could not be set: contact the website administrator",
				NULL);
	}

	/* cleanup */
	json_decref(proto_state);
	oidc_jwt_destroy(jwt);

	/* check that we've actually authenticated a user; functions as error handling for oidc_get_remote_user */
	if (r->user == NULL)
		return HTTP_UNAUTHORIZED;

	/* log the successful response */
	oidc_debug(r,
			"session created and stored, returning to original URL: %s, original method: %s",
			original_url, original_method);

	/* check whether form post data was preserved; if so restore it */
	if (apr_strnatcmp(original_method, OIDC_METHOD_FORM_POST) == 0) {
		return oidc_request_post_preserved_restore(r, original_url);
	}

	/* now we've authenticated the user so go back to the URL that he originally tried to access */
	apr_table_add(r->headers_out, "Location", original_url);

	/* do the actual redirect to the original URL */
	return HTTP_MOVED_TEMPORARILY;
}