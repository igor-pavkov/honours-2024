static void cli_start_connection_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_start_connection_state *state = tevent_req_data(
		req, struct cli_start_connection_state);
	NTSTATUS status;

	status = smbXcli_negprot_recv(subreq);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	if (smbXcli_conn_protocol(state->cli->conn) >= PROTOCOL_SMB2_02) {
		/* Ensure we ask for some initial credits. */
		smb2cli_conn_set_max_credits(state->cli->conn,
					     DEFAULT_SMB2_MAX_CREDITS);
	}

	tevent_req_done(req);
}