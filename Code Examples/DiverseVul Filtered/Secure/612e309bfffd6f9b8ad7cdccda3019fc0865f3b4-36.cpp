static int oidc_authenticate_user(request_rec *r, oidc_cfg *c,
		oidc_provider_t *provider, const char *original_url,
		const char *login_hint, const char *id_token_hint, const char *prompt,
		const char *auth_request_params) {

	oidc_debug(r, "enter");

	if (provider == NULL) {

		// TODO: should we use an explicit redirect to the discovery endpoint (maybe a "discovery" param to the redirect_uri)?
		if (c->metadata_dir != NULL)
			return oidc_discovery(r, c);

		/* we're not using multiple OP's configured in a metadata directory, pick the statically configured OP */
		if (oidc_provider_static_config(r, c, &provider) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* generate the random nonce value that correlates requests and responses */
	char *nonce = NULL;
	if (oidc_proto_generate_nonce(r, &nonce, OIDC_PROTO_NONCE_LENGTH) == FALSE)
		return HTTP_INTERNAL_SERVER_ERROR;

	char *code_verifier = NULL;
	char *code_challenge = NULL;

	if ((oidc_util_spaced_string_contains(r->pool, provider->response_type,
			"code") == TRUE) && (provider->pkce_method != NULL)) {

		/* generate the code verifier value that correlates authorization requests and code exchange requests */
		if (oidc_proto_generate_code_verifier(r, &code_verifier,
				OIDC_PROTO_CODE_VERIFIER_LENGTH) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;

		/* generate the PKCE code challenge */
		if (oidc_proto_generate_code_challenge(r, code_verifier,
				&code_challenge, provider->pkce_method) == FALSE)
			return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* create the state between request/response */
	json_t *proto_state = json_object();
	json_object_set_new(proto_state, "original_url", json_string(original_url));
	json_object_set_new(proto_state, "original_method",
			json_string(oidc_original_request_method(r, c, TRUE)));
	json_object_set_new(proto_state, "issuer", json_string(provider->issuer));
	json_object_set_new(proto_state, "response_type",
			json_string(provider->response_type));
	json_object_set_new(proto_state, "nonce", json_string(nonce));
	json_object_set_new(proto_state, "timestamp",
			json_integer(apr_time_sec(apr_time_now())));
	if (provider->response_mode)
		json_object_set_new(proto_state, "response_mode",
				json_string(provider->response_mode));
	if (prompt)
		json_object_set_new(proto_state, "prompt", json_string(prompt));
	if (code_verifier)
		json_object_set_new(proto_state, "code_verifier",
				json_string(code_verifier));

	/* get a hash value that fingerprints the browser concatenated with the random input */
	char *state = oidc_get_browser_state_hash(r, nonce);

	/* create state that restores the context when the authorization response comes in; cryptographically bind it to the browser */
	if (oidc_authorization_request_set_cookie(r, c, state, proto_state) == FALSE)
		return HTTP_INTERNAL_SERVER_ERROR;

	/*
	 * printout errors if Cookie settings are not going to work
	 */
	apr_uri_t o_uri;
	memset(&o_uri, 0, sizeof(apr_uri_t));
	apr_uri_t r_uri;
	memset(&r_uri, 0, sizeof(apr_uri_t));
	apr_uri_parse(r->pool, original_url, &o_uri);
	apr_uri_parse(r->pool, c->redirect_uri, &r_uri);
	if ((apr_strnatcmp(o_uri.scheme, r_uri.scheme) != 0)
			&& (apr_strnatcmp(r_uri.scheme, "https") == 0)) {
		oidc_error(r,
				"the URL scheme (%s) of the configured OIDCRedirectURI does not match the URL scheme of the URL being accessed (%s): the \"state\" and \"session\" cookies will not be shared between the two!",
				r_uri.scheme, o_uri.scheme);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (c->cookie_domain == NULL) {
		if (apr_strnatcmp(o_uri.hostname, r_uri.hostname) != 0) {
			char *p = strstr(o_uri.hostname, r_uri.hostname);
			if ((p == NULL) || (apr_strnatcmp(r_uri.hostname, p) != 0)) {
				oidc_error(r,
						"the URL hostname (%s) of the configured OIDCRedirectURI does not match the URL hostname of the URL being accessed (%s): the \"state\" and \"session\" cookies will not be shared between the two!",
						r_uri.hostname, o_uri.hostname);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		}
	} else {
		if (!oidc_util_cookie_domain_valid(r_uri.hostname, c->cookie_domain)) {
			oidc_error(r,
					"the domain (%s) configured in OIDCCookieDomain does not match the URL hostname (%s) of the URL being accessed (%s): setting \"state\" and \"session\" cookies will not work!!",
					c->cookie_domain, o_uri.hostname, original_url);
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}

	/* send off to the OpenID Connect Provider */
	// TODO: maybe show intermediate/progress screen "redirecting to"
	return oidc_proto_authorization_request(r, provider, login_hint,
			c->redirect_uri, state, proto_state, id_token_hint, code_challenge,
			auth_request_params);
}