static const char *oidc_retrieve_claims_from_userinfo_endpoint(request_rec *r,
		oidc_cfg *c, oidc_provider_t *provider, const char *access_token,
		oidc_session_t *session, char *id_token_sub) {

	oidc_debug(r, "enter");

	/* see if a userinfo endpoint is set, otherwise there's nothing to do for us */
	if (provider->userinfo_endpoint_url == NULL) {
		oidc_debug(r,
				"not retrieving userinfo claims because userinfo_endpoint is not set");
		return NULL;
	}

	/* see if there's an access token, otherwise we can't call the userinfo endpoint at all */
	if (access_token == NULL) {
		oidc_debug(r,
				"not retrieving userinfo claims because access_token is not provided");
		return NULL;
	}

	if ((id_token_sub == NULL) && (session != NULL)) {

		// when refreshing claims from the userinfo endpoint

		const char *s_id_token_claims = NULL;
		oidc_session_get(r, session, OIDC_IDTOKEN_CLAIMS_SESSION_KEY,
				&s_id_token_claims);

		if (s_id_token_claims == NULL) {
			oidc_error(r, "no id_token claims provided");
			return NULL;
		}

		json_error_t json_error;
		json_t *id_token_claims = json_loads(s_id_token_claims, 0, &json_error);

		if (id_token_claims == NULL) {
			oidc_error(r, "JSON parsing (json_loads) failed: %s (%s)",
					json_error.text, s_id_token_claims);
			return NULL;
		}

		oidc_jose_get_string(r->pool, id_token_claims, "sub", FALSE, &id_token_sub, NULL);
	}

	// TODO: return code should indicate whether the token expired or some other error occurred
	// TODO: long-term: session storage should be JSON (with explicit types and less conversion, using standard routines)

	/* try to get claims from the userinfo endpoint using the provided access token */
	const char *result = NULL;
	if (oidc_proto_resolve_userinfo(r, c, provider, id_token_sub, access_token,
			&result) == FALSE) {

		/* see if we have an existing session and we are refreshing the user info claims */
		if (session != NULL) {

			/* first call to user info endpoint failed, but the access token may have just expired, so refresh it */
			char *access_token = NULL;
			if (oidc_refresh_access_token(r, c, session, provider,
					&access_token) == TRUE) {

				/* try again with the new access token */
				if (oidc_proto_resolve_userinfo(r, c, provider, id_token_sub, access_token,
						&result) == FALSE) {

					oidc_error(r,
							"resolving user info claims with the refreshed access token failed, nothing will be stored in the session");
					result = NULL;

				}

			} else {

				oidc_warn(r,
						"refreshing access token failed, claims will not be retrieved/refreshed from the userinfo endpoint");
				result = NULL;

			}

		} else {

			oidc_error(r,
					"resolving user info claims with the existing/provided access token failed, nothing will be stored in the session");
			result = NULL;

		}
	}

	return result;
}