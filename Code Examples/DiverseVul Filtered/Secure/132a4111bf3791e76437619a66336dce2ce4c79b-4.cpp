int oidc_content_handler(request_rec *r) {
	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	int rc = DECLINED;
	if (oidc_util_request_matches_url(r, oidc_get_redirect_uri(r, c))) {

		if (oidc_util_request_has_parameter(r,
				OIDC_REDIRECT_URI_REQUEST_INFO)) {

			oidc_session_t *session = NULL;
			oidc_session_load(r, &session);

			/* handle request for session info */
			rc = oidc_handle_info_request(r, c, session);

			/* free resources allocated for the session */
			oidc_session_free(r, session);

		}

	}
	return rc;
}