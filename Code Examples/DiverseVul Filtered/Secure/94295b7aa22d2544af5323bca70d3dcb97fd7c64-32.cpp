static void cli_session_setup_gensec_ready(struct tevent_req *req)
{
	struct cli_session_setup_gensec_state *state =
		tevent_req_data(req,
		struct cli_session_setup_gensec_state);
	const char *server_domain = NULL;
	NTSTATUS status;

	if (state->blob_in.length != 0) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (state->blob_out.length != 0) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	/*
	 * gensec_ntlmssp_server_domain() returns NULL
	 * if NTLMSSP is not used.
	 *
	 * We can remove this later
	 * and leave the server domain empty for SMB2 and above
	 * in future releases.
	 */
	server_domain = gensec_ntlmssp_server_domain(
				state->auth_generic->gensec_security);

	if (state->cli->server_domain[0] == '\0' && server_domain != NULL) {
		TALLOC_FREE(state->cli->server_domain);
		state->cli->server_domain = talloc_strdup(state->cli,
					server_domain);
		if (state->cli->server_domain == NULL) {
			tevent_req_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}
	}

	if (state->is_anonymous) {
		/*
		 * Windows server does not set the
		 * SMB2_SESSION_FLAG_IS_NULL flag.
		 *
		 * This fix makes sure we do not try
		 * to verify a signature on the final
		 * session setup response.
		 */
		tevent_req_done(req);
		return;
	}

	status = gensec_session_key(state->auth_generic->gensec_security,
				    state, &state->session_key);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	if (smbXcli_conn_protocol(state->cli->conn) >= PROTOCOL_SMB2_02) {
		struct smbXcli_session *session = state->cli->smb2.session;

		status = smb2cli_session_set_session_key(session,
							 state->session_key,
							 state->recv_iov);
		if (tevent_req_nterror(req, status)) {
			return;
		}
	} else {
		struct smbXcli_session *session = state->cli->smb1.session;
		bool active;

		status = smb1cli_session_set_session_key(session,
							 state->session_key);
		if (tevent_req_nterror(req, status)) {
			return;
		}

		active = smb1cli_conn_activate_signing(state->cli->conn,
						       state->session_key,
						       data_blob_null);
		if (active) {
			bool ok;

			ok = smb1cli_conn_check_signing(state->cli->conn,
							state->inbuf, 1);
			if (!ok) {
				tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
				return;
			}
		}
	}

	tevent_req_done(req);
}