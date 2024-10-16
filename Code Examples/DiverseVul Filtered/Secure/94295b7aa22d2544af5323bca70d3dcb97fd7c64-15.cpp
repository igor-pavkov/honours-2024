struct tevent_req *cli_session_setup_guest_create(TALLOC_CTX *mem_ctx,
						  struct tevent_context *ev,
						  struct cli_state *cli,
						  struct tevent_req **psmbreq)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_guest_state *state;
	uint16_t *vwv;
	uint8_t *bytes;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_guest_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;
	vwv = state->vwv;

	SCVAL(vwv+0, 0, 0xFF);
	SCVAL(vwv+0, 1, 0);
	SSVAL(vwv+1, 0, 0);
	SSVAL(vwv+2, 0, CLI_BUFFER_SIZE);
	SSVAL(vwv+3, 0, 2);
	SSVAL(vwv+4, 0, cli_state_get_vc_num(cli));
	SIVAL(vwv+5, 0, smb1cli_conn_server_session_key(cli->conn));
	SSVAL(vwv+7, 0, 0);
	SSVAL(vwv+8, 0, 0);
	SSVAL(vwv+9, 0, 0);
	SSVAL(vwv+10, 0, 0);
	SIVAL(vwv+11, 0, cli_session_setup_capabilities(cli, 0));

	bytes = talloc_array(state, uint8_t, 0);

	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "",  1, /* username */
				   NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "", 1, /* workgroup */
				   NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Unix", 5, NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Samba", 6, NULL);

	if (bytes == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}

	state->bytes.iov_base = (void *)bytes;
	state->bytes.iov_len = talloc_get_size(bytes);

	subreq = cli_smb_req_create(state, ev, cli, SMBsesssetupX, 0, 13, vwv,
				    1, &state->bytes);
	if (subreq == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}
	tevent_req_set_callback(subreq, cli_session_setup_guest_done, req);
	*psmbreq = subreq;
	return req;
}