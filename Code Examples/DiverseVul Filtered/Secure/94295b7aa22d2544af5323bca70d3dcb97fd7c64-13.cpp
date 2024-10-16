static struct tevent_req *cli_raw_tcon_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev, struct cli_state *cli,
	const char *service, const char *pass, const char *dev)
{
	struct tevent_req *req, *subreq;
	struct cli_raw_tcon_state *state;
	uint8_t *bytes;

	req = tevent_req_create(mem_ctx, &state, struct cli_raw_tcon_state);
	if (req == NULL) {
		return NULL;
	}

	if (!lp_client_plaintext_auth() && (*pass)) {
		DEBUG(1, ("Server requested PLAINTEXT password but 'client plaintext auth = no'"
			  " or 'client ntlmv2 auth = yes'\n"));
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return tevent_req_post(req, ev);
	}

	bytes = talloc_array(state, uint8_t, 0);
	bytes = smb_bytes_push_bytes(bytes, 4, NULL, 0);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   service, strlen(service)+1, NULL);
	bytes = smb_bytes_push_bytes(bytes, 4, NULL, 0);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   pass, strlen(pass)+1, NULL);
	bytes = smb_bytes_push_bytes(bytes, 4, NULL, 0);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   dev, strlen(dev)+1, NULL);

	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}

	subreq = cli_smb_send(state, ev, cli, SMBtcon, 0, 0, NULL,
			      talloc_get_size(bytes), bytes);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_raw_tcon_done, req);
	return req;
}