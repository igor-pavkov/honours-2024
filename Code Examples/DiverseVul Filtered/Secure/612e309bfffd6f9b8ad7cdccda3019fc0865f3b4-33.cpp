static apr_byte_t oidc_authorization_request_set_cookie(request_rec *r,
		oidc_cfg *c, const char *state, json_t *proto_state) {
	/*
	 * create a cookie consisting of 8 elements:
	 * random value, original URL, original method, issuer, response_type, response_mod, prompt and timestamp
	 * encoded as JSON
	 */

	/* encrypt the resulting JSON value  */
	char *cookieValue = NULL;

	if (oidc_util_jwt_create(r, c->crypto_passphrase, proto_state,
			&cookieValue) == FALSE)
		return FALSE;

	/* clean expired state cookies to avoid pollution */
	oidc_clean_expired_state_cookies(r, c);

	/* assemble the cookie name for the state cookie */
	const char *cookieName = oidc_get_state_cookie_name(r, state);

	/* set it as a cookie */
	oidc_util_set_cookie(r, cookieName, cookieValue, -1);

	//free(s_value);

	return TRUE;
}