static struct tevent_req *cli_sesssetup_blob_send(TALLOC_CTX *mem_ctx,
						  struct tevent_context *ev,
						  struct cli_state *cli,
						  DATA_BLOB blob)
{
	struct tevent_req *req, *subreq;
	struct cli_sesssetup_blob_state *state;
	uint32_t usable_space;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_sesssetup_blob_state);
	if (req == NULL) {
		return NULL;
	}
	state->ev = ev;
	state->blob = blob;
	state->cli = cli;

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		usable_space = UINT16_MAX;
	} else {
		usable_space = cli_state_available_size(cli,
				BASE_SESSSETUP_BLOB_PACKET_SIZE);
	}

	if (usable_space == 0) {
		DEBUG(1, ("cli_session_setup_blob: cli->max_xmit too small "
			  "(not possible to send %u bytes)\n",
			  BASE_SESSSETUP_BLOB_PACKET_SIZE + 1));
		tevent_req_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return tevent_req_post(req, ev);
	}
	state->max_blob_size = MIN(usable_space, 0xFFFF);

	if (!cli_sesssetup_blob_next(state, &subreq)) {
		tevent_req_nterror(req, NT_STATUS_NO_MEMORY);
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_sesssetup_blob_done, req);
	return req;
}