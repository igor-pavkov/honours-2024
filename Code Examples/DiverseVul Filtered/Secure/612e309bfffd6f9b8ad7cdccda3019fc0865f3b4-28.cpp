static apr_byte_t oidc_set_app_claims(request_rec *r,
		const oidc_cfg * const cfg, oidc_session_t *session,
		const char *s_claims) {

	json_t *j_claims = NULL;

	/* decode the string-encoded attributes in to a JSON structure */
	if (s_claims != NULL) {
		json_error_t json_error;
		j_claims = json_loads(s_claims, 0, &json_error);

		if (j_claims == NULL) {
			/* whoops, JSON has been corrupted */
			oidc_error(r,
					"unable to parse \"%s\" JSON stored in the session: %s",
					s_claims, json_error.text);

			return FALSE;
		}
	}

	/* set the resolved claims a HTTP headers for the application */
	if (j_claims != NULL) {
		oidc_util_set_app_infos(r, j_claims, cfg->claim_prefix,
				cfg->claim_delimiter, oidc_cfg_dir_pass_info_in_headers(r),
				oidc_cfg_dir_pass_info_in_envvars(r));

		/* release resources */
		json_decref(j_claims);
	}

	return TRUE;
}