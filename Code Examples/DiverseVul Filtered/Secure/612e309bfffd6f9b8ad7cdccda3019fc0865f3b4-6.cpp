static void oidc_copy_tokens_to_request_state(request_rec *r,
		oidc_session_t *session, const char **s_id_token, const char **s_claims) {

	const char *id_token = NULL, *claims = NULL;

	oidc_session_get(r, session, OIDC_IDTOKEN_CLAIMS_SESSION_KEY, &id_token);
	oidc_session_get(r, session, OIDC_CLAIMS_SESSION_KEY, &claims);

	oidc_debug(r, "id_token=%s claims=%s", id_token, claims);

	if (id_token != NULL) {
		oidc_request_state_set(r, OIDC_IDTOKEN_CLAIMS_SESSION_KEY, id_token);
		if (s_id_token != NULL)
			*s_id_token = id_token;
	}

	if (claims != NULL) {
		oidc_request_state_set(r, OIDC_CLAIMS_SESSION_KEY, claims);
		if (s_claims != NULL)
			*s_claims = claims;
	}
}