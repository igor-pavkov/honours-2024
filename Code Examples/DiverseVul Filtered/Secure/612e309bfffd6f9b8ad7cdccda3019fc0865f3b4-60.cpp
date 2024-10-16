static apr_byte_t oidc_unsolicited_proto_state(request_rec *r, oidc_cfg *c,
		const char *state, json_t **proto_state) {

	char *alg = NULL;
	oidc_debug(r, "enter: state header=%s",
			oidc_proto_peek_jwt_header(r, state, &alg));

	oidc_jose_error_t err;
	oidc_jwk_t *jwk = NULL;
	if (oidc_util_create_symmetric_key(r, c->provider.client_secret,
			oidc_alg2keysize(alg), "sha256",
			TRUE, &jwk) == FALSE)
		return FALSE;

	oidc_jwt_t *jwt = NULL;
	if (oidc_jwt_parse(r->pool, state, &jwt,
			oidc_util_merge_symmetric_key(r->pool, c->private_keys, jwk),
			&err) == FALSE) {
		oidc_error(r,
				"could not parse JWT from state: invalid unsolicited response: %s",
				oidc_jose_e2s(r->pool, err));
		return FALSE;
	}

	oidc_jwk_destroy(jwk);
	oidc_debug(r, "successfully parsed JWT from state");

	if (jwt->payload.iss == NULL) {
		oidc_error(r, "no \"iss\" could be retrieved from JWT state, aborting");
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	oidc_provider_t *provider = oidc_get_provider_for_issuer(r, c,
			jwt->payload.iss, FALSE);
	if (provider == NULL) {
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	/* validate the state JWT, validating optional exp + iat */
	if (oidc_proto_validate_jwt(r, jwt, provider->issuer, FALSE, FALSE,
			provider->idtoken_iat_slack) == FALSE) {
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	char *rfp = NULL;
	if (oidc_jose_get_string(r->pool, jwt->payload.value.json, "rfp", TRUE,
			&rfp, &err) == FALSE) {
		oidc_error(r,
				"no \"rfp\" claim could be retrieved from JWT state, aborting: %s",
				oidc_jose_e2s(r->pool, err));
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	if (apr_strnatcmp(rfp, "iss") != 0) {
		oidc_error(r, "\"rfp\" (%s) does not match \"iss\", aborting", rfp);
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	char *target_link_uri = NULL;
	oidc_jose_get_string(r->pool, jwt->payload.value.json, "target_link_uri",
			FALSE, &target_link_uri, NULL);
	if (target_link_uri == NULL) {
		if (c->default_sso_url == NULL) {
			oidc_error(r,
					"no \"target_link_uri\" claim could be retrieved from JWT state and no OIDCDefaultURL is set, aborting");
			oidc_jwt_destroy(jwt);
			return FALSE;
		}
		target_link_uri = c->default_sso_url;
	}

	if (c->metadata_dir != NULL) {
		if ((oidc_metadata_get(r, c, jwt->payload.iss, &provider, FALSE)
				== FALSE) || (provider == NULL)) {
			oidc_error(r, "no provider metadata found for provider \"%s\"",
					jwt->payload.iss);
			oidc_jwt_destroy(jwt);
			return FALSE;
		}
	}

	char *jti = NULL;
	oidc_jose_get_string(r->pool, jwt->payload.value.json, "jti", FALSE, &jti,
			NULL);
	if (jti == NULL) {
		char *cser = oidc_jwt_serialize(r->pool, jwt, &err);
		if (cser == NULL)
			return FALSE;
		if (oidc_util_hash_string_and_base64url_encode(r, "sha256", cser,
				&jti) == FALSE) {
			oidc_error(r,
					"oidc_util_hash_string_and_base64url_encode returned an error");
			return FALSE;
		}
	}

	const char *replay = NULL;
	c->cache->get(r, OIDC_CACHE_SECTION_JTI, jti, &replay);
	if (replay != NULL) {
		oidc_error(r,
				"the jti value (%s) passed in the browser state was found in the cache already; possible replay attack!?",
				jti);
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	/* jti cache duration is the configured replay prevention window for token issuance plus 10 seconds for safety */
	apr_time_t jti_cache_duration = apr_time_from_sec(
			provider->idtoken_iat_slack * 2 + 10);

	/* store it in the cache for the calculated duration */
	c->cache->set(r, OIDC_CACHE_SECTION_JTI, jti, jti,
			apr_time_now() + jti_cache_duration);

	oidc_debug(r,
			"jti \"%s\" validated successfully and is now cached for %" APR_TIME_T_FMT " seconds",
			jti, apr_time_sec(jti_cache_duration));

	jwk = NULL;
	if (oidc_util_create_symmetric_key(r, c->provider.client_secret, 0,
			NULL, TRUE, &jwk) == FALSE)
		return FALSE;

	oidc_jwks_uri_t jwks_uri = { provider->jwks_uri,
			provider->jwks_refresh_interval, provider->ssl_validate_server };
	if (oidc_proto_jwt_verify(r, c, jwt, &jwks_uri,
			oidc_util_merge_symmetric_key(r->pool, NULL, jwk)) == FALSE) {
		oidc_error(r, "state JWT could not be validated, aborting");
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	oidc_jwk_destroy(jwk);
	oidc_debug(r, "successfully verified state JWT");

	*proto_state = json_object();
	json_object_set_new(*proto_state, "issuer", json_string(jwt->payload.iss));
	json_object_set_new(*proto_state, "original_url",
			json_string(target_link_uri));
	json_object_set_new(*proto_state, "original_method", json_string("get"));
	json_object_set_new(*proto_state, "response_mode",
			json_string(provider->response_mode));
	json_object_set_new(*proto_state, "response_type",
			json_string(provider->response_type));
	json_object_set_new(*proto_state, "timestamp",
			json_integer(apr_time_sec(apr_time_now())));

	oidc_jwt_destroy(jwt);

	return TRUE;
}