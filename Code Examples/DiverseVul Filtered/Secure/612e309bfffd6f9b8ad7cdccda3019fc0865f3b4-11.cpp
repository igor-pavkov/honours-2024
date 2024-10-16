static apr_byte_t oidc_restore_proto_state(request_rec *r, oidc_cfg *c,
		const char *state, json_t **proto_state) {

	oidc_debug(r, "enter");

	/* clean expired state cookies to avoid pollution */
	oidc_clean_expired_state_cookies(r, c);

	const char *cookieName = oidc_get_state_cookie_name(r, state);

	/* get the state cookie value first */
	char *cookieValue = oidc_util_get_cookie(r, cookieName);
	if (cookieValue == NULL) {
		oidc_error(r, "no \"%s\" state cookie found", cookieName);
		return oidc_unsolicited_proto_state(r, c, state, proto_state);
	}

	/* clear state cookie because we don't need it anymore */
	oidc_util_set_cookie(r, cookieName, "", 0);

	*proto_state = oidc_get_state_from_cookie(r, c, cookieValue);
	if (*proto_state == NULL)
		return FALSE;

	json_t *v = json_object_get(*proto_state, "nonce");

	/* calculate the hash of the browser fingerprint concatenated with the nonce */
	char *calc = oidc_get_browser_state_hash(r, json_string_value(v));
	/* compare the calculated hash with the value provided in the authorization response */
	if (apr_strnatcmp(calc, state) != 0) {
		oidc_error(r,
				"calculated state from cookie does not match state parameter passed back in URL: \"%s\" != \"%s\"",
				state, calc);
		json_decref(*proto_state);
		return FALSE;
	}

	v = json_object_get(*proto_state, "timestamp");
	apr_time_t now = apr_time_sec(apr_time_now());

	/* check that the timestamp is not beyond the valid interval */
	if (now > json_integer_value(v) + c->state_timeout) {
		oidc_error(r, "state has expired");
		json_decref(*proto_state);
		return FALSE;
	}

	/* add the state */
	json_object_set_new(*proto_state, "state", json_string(state));

	char *s_value = json_dumps(*proto_state, JSON_ENCODE_ANY);
	oidc_debug(r, "restored state: %s", s_value);
	free(s_value);

	/* we've made it */
	return TRUE;
}