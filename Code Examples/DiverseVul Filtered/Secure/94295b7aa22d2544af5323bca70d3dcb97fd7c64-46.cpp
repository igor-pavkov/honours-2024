static struct tevent_req *cli_session_setup_plain_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	struct cli_state *cli,
	const char *user, const char *pass, const char *workgroup)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_plain_state *state;
	uint16_t *vwv;
	uint8_t *bytes;
	size_t passlen;
	char *version;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_plain_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;
	state->user = user;
	vwv = state->vwv;

	SCVAL(vwv+0, 0, 0xff);
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
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), pass, strlen(pass)+1,
				   &passlen);
	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}
	SSVAL(vwv + (smbXcli_conn_use_unicode(cli->conn) ? 8 : 7), 0, passlen);

	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   user, strlen(user)+1, NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   workgroup, strlen(workgroup)+1, NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   "Unix", 5, NULL);

	version = talloc_asprintf(talloc_tos(), "Samba %s",
				  samba_version_string());
	if (tevent_req_nomem(version, req)){
		return tevent_req_post(req, ev);
	}
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   version, strlen(version)+1, NULL);
	TALLOC_FREE(version);

	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}

	subreq = cli_smb_send(state, ev, cli, SMBsesssetupX, 0, 13, vwv,
			      talloc_get_size(bytes), bytes);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_session_setup_plain_done, req);
	return req;
}