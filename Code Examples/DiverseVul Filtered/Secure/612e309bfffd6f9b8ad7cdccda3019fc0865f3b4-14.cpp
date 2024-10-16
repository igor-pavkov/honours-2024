static apr_byte_t oidc_handle_flows(request_rec *r, oidc_cfg *c,
		json_t *proto_state, oidc_provider_t *provider, apr_table_t *params,
		const char *response_mode, oidc_jwt_t **jwt) {

	apr_byte_t rc = FALSE;

	const char *requested_response_type = json_string_value(
			json_object_get(proto_state, "response_type"));

	/* handle the requested response type/mode */
	if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"code id_token token")) {
		rc = oidc_proto_authorization_response_code_idtoken_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"code id_token")) {
		rc = oidc_proto_authorization_response_code_idtoken(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"code token")) {
		rc = oidc_proto_handle_authorization_response_code_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"code")) {
		rc = oidc_proto_handle_authorization_response_code(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"id_token token")) {
		rc = oidc_proto_handle_authorization_response_idtoken_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			"id_token")) {
		rc = oidc_proto_handle_authorization_response_idtoken(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else {
		oidc_error(r, "unsupported response type: \"%s\"",
				requested_response_type);
	}

	if ((rc == FALSE) && (*jwt != NULL)) {
		oidc_jwt_destroy(*jwt);
		*jwt = NULL;
	}

	return rc;
}