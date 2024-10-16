static struct tevent_req *cli_session_setup_gensec_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev, struct cli_state *cli,
	const char *user, const char *pass, const char *domain,
	enum credentials_use_kerberos krb5_state,
	const char *target_service,
	const char *target_hostname,
	const char *target_principal)
{
	struct tevent_req *req;
	struct cli_session_setup_gensec_state *state;
	NTSTATUS status;
	bool use_spnego_principal = lp_client_use_spnego_principal();

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_gensec_state);
	if (req == NULL) {
		return NULL;
	}
	state->ev = ev;
	state->cli = cli;

	talloc_set_destructor(
		state, cli_session_setup_gensec_state_destructor);

	if (user == NULL || strlen(user) == 0) {
		if (pass != NULL && strlen(pass) == 0) {
			/*
			 * some callers pass "" as no password
			 *
			 * gensec only handles NULL as no password.
			 */
			pass = NULL;
		}
	}

	status = auth_generic_client_prepare(state, &state->auth_generic);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}

	gensec_want_feature(state->auth_generic->gensec_security,
			    GENSEC_FEATURE_SESSION_KEY);
	if (cli->use_ccache) {
		gensec_want_feature(state->auth_generic->gensec_security,
				    GENSEC_FEATURE_NTLM_CCACHE);
		if (pass != NULL && strlen(pass) == 0) {
			/*
			 * some callers pass "" as no password
			 *
			 * GENSEC_FEATURE_NTLM_CCACHE only handles
			 * NULL as no password.
			 */
			pass = NULL;
		}
	}

	status = auth_generic_set_username(state->auth_generic, user);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}

	status = auth_generic_set_domain(state->auth_generic, domain);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}

	if (cli->pw_nt_hash) {
		struct samr_Password nt_hash;
		size_t converted;
		bool ok;

		if (pass == NULL) {
			tevent_req_nterror(req, NT_STATUS_INVALID_PARAMETER_MIX);
			return tevent_req_post(req, ev);
		}

		converted = strhex_to_str((char *)nt_hash.hash,
					  sizeof(nt_hash.hash),
					  pass, strlen(pass));
		if (converted != sizeof(nt_hash.hash)) {
			tevent_req_nterror(req, NT_STATUS_INVALID_PARAMETER_MIX);
			return tevent_req_post(req, ev);
		}

		ok = cli_credentials_set_nt_hash(state->auth_generic->credentials,
						 &nt_hash, CRED_SPECIFIED);
		if (!ok) {
			tevent_req_oom(req);
			return tevent_req_post(req, ev);
		}
	} else {
		status = auth_generic_set_password(state->auth_generic, pass);
		if (tevent_req_nterror(req, status)) {
			return tevent_req_post(req, ev);
		}
	}

	cli_credentials_set_kerberos_state(state->auth_generic->credentials,
					   krb5_state);

	if (krb5_state == CRED_DONT_USE_KERBEROS) {
		use_spnego_principal = false;
	}

	if (target_service != NULL) {
		status = gensec_set_target_service(
				state->auth_generic->gensec_security,
				target_service);
		if (tevent_req_nterror(req, status)) {
			return tevent_req_post(req, ev);
		}
	}

	if (target_hostname != NULL) {
		status = gensec_set_target_hostname(
				state->auth_generic->gensec_security,
				target_hostname);
		if (tevent_req_nterror(req, status)) {
			return tevent_req_post(req, ev);
		}
	}

	if (target_principal != NULL) {
		status = gensec_set_target_principal(
				state->auth_generic->gensec_security,
				target_principal);
		if (tevent_req_nterror(req, status)) {
			return tevent_req_post(req, ev);
		}
		use_spnego_principal = false;
	} else if (target_service != NULL && target_hostname != NULL) {
		use_spnego_principal = false;
	}

	if (use_spnego_principal) {
		const DATA_BLOB *b;
		b = smbXcli_conn_server_gss_blob(cli->conn);
		if (b != NULL) {
			state->blob_in = *b;
		}
	}

	state->is_anonymous = cli_credentials_is_anonymous(state->auth_generic->credentials);

	status = auth_generic_client_start(state->auth_generic,
					   GENSEC_OID_SPNEGO);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		state->cli->smb2.session = smbXcli_session_create(cli,
								  cli->conn);
		if (tevent_req_nomem(state->cli->smb2.session, req)) {
			return tevent_req_post(req, ev);
		}
	}

	cli_session_setup_gensec_local_next(req);
	if (!tevent_req_is_in_progress(req)) {
		return tevent_req_post(req, ev);
	}

	return req;
}