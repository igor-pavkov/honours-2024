static void cli_sesssetup_blob_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_sesssetup_blob_state *state = tevent_req_data(
		req, struct cli_sesssetup_blob_state);
	struct cli_state *cli = state->cli;
	uint8_t wct;
	uint16_t *vwv;
	uint32_t num_bytes;
	uint8_t *bytes;
	NTSTATUS status;
	uint8_t *p;
	uint16_t blob_length;
	uint8_t *in;
	uint8_t *inhdr;
	ssize_t ret;

	if (smbXcli_conn_protocol(state->cli->conn) >= PROTOCOL_SMB2_02) {
		status = smb2cli_session_setup_recv(subreq, state,
						    &state->recv_iov,
						    &state->ret_blob);
	} else {
		status = cli_smb_recv(subreq, state, &in, 4, &wct, &vwv,
				      &num_bytes, &bytes);
		TALLOC_FREE(state->buf);
	}
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)
	    && !NT_STATUS_EQUAL(status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		tevent_req_nterror(req, status);
		return;
	}

	state->status = status;

	if (smbXcli_conn_protocol(state->cli->conn) >= PROTOCOL_SMB2_02) {
		goto next;
	}

	state->inbuf = in;
	inhdr = in + NBT_HDR_SIZE;
	cli_state_set_uid(state->cli, SVAL(inhdr, HDR_UID));
	smb1cli_session_set_action(cli->smb1.session, SVAL(vwv+2, 0));

	blob_length = SVAL(vwv+3, 0);
	if (blob_length > num_bytes) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}
	state->ret_blob = data_blob_const(bytes, blob_length);

	p = bytes + blob_length;

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

next:
	if (state->blob.length != 0) {
		/*
		 * More to send
		 */
		if (!cli_sesssetup_blob_next(state, &subreq)) {
			tevent_req_oom(req);
			return;
		}
		tevent_req_set_callback(subreq, cli_sesssetup_blob_done, req);
		return;
	}
	tevent_req_done(req);
}