static void oidc_authz_get_claims_and_idtoken(request_rec *r, json_t **claims,
		json_t **id_token) {
	const char *s_claims = oidc_request_state_get(r, OIDC_CLAIMS_SESSION_KEY);
	const char *s_id_token = oidc_request_state_get(r,
			OIDC_IDTOKEN_CLAIMS_SESSION_KEY);
	json_error_t json_error;
	if (s_claims != NULL) {
		*claims = json_loads(s_claims, 0, &json_error);
		if (*claims == NULL) {
			oidc_error(r, "could not restore claims from request state: %s",
					json_error.text);
		}
	}
	if (s_id_token != NULL) {
		*id_token = json_loads(s_id_token, 0, &json_error);
		if (*id_token == NULL) {
			oidc_error(r, "could not restore id_token from request state: %s",
					json_error.text);
		}
	}
}