static void cli_session_setup_nt1_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_session_setup_nt1_state *state = tevent_req_data(
		req, struct cli_session_setup_nt1_state);
	struct cli_state *cli = state->cli;
	uint32_t num_bytes;
	uint8_t *in;
	uint8_t *inhdr;
	uint8_t *bytes;
	uint8_t *p;
	NTSTATUS status;
	ssize_t ret;
	uint8_t wct;
	uint16_t *vwv;

	status = cli_smb_recv(subreq, state, &in, 3, &wct, &vwv,
			      &num_bytes, &bytes);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}

	inhdr = in + NBT_HDR_SIZE;
	p = bytes;

	cli_state_set_uid(state->cli, SVAL(inhdr, HDR_UID));
	smb1cli_session_set_action(cli->smb1.session, SVAL(vwv+2, 0));

	status = smb_bytes_talloc_string(cli,
					inhdr,
					&cli->server_os,
					p,
					bytes+num_bytes-p,
					&ret);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}
	p += ret;

	status = smb_bytes_talloc_string(cli,
					inhdr,
					&cli->server_type,
					p,
					bytes+num_bytes-p,
					&ret);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}
	p += ret;

	status = smb_bytes_talloc_string(cli,
					inhdr,
					&cli->server_domain,
					p,
					bytes+num_bytes-p,
					&ret);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}
	p += ret;

	if (smb1cli_conn_activate_signing(cli->conn, state->session_key, state->response)
	    && !smb1cli_conn_check_signing(cli->conn, (uint8_t *)in, 1)) {
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}
	if (state->session_key.data) {
		struct smbXcli_session *session = state->cli->smb1.session;

		status = smb1cli_session_set_session_key(session,
				state->session_key);
		if (tevent_req_nterror(req, status)) {
			return;
		}
	}
	tevent_req_done(req);
}