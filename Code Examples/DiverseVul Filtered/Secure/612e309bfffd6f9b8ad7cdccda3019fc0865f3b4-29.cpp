static int oidc_authorization_response_error(request_rec *r, oidc_cfg *c,
		json_t *proto_state, const char *error, const char *error_description) {
	const char *prompt =
			json_object_get(proto_state, "prompt") ?
					apr_pstrdup(r->pool,
							json_string_value(
									json_object_get(proto_state, "prompt"))) :
									NULL;
	json_decref(proto_state);
	if ((prompt != NULL) && (apr_strnatcmp(prompt, "none") == 0)) {
		return oidc_session_redirect_parent_window_to_logout(r, c);
	}
	return oidc_util_html_send_error(r, c->error_template,
			apr_psprintf(r->pool, "OpenID Connect Provider error: %s", error),
			error_description, DONE);
}