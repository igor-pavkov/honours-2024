static struct tevent_req *cli_tree_connect_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev, struct cli_state *cli,
	const char *share, const char *dev, const char *pass, int passlen)
{
	struct tevent_req *req, *subreq;
	struct cli_tree_connect_state *state;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_tree_connect_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;

	cli->share = talloc_strdup(cli, share);
	if (tevent_req_nomem(cli->share, req)) {
		return tevent_req_post(req, ev);
	}

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		char *unc;

		cli->smb2.tcon = smbXcli_tcon_create(cli);
		if (tevent_req_nomem(cli->smb2.tcon, req)) {
			return tevent_req_post(req, ev);
		}

		unc = talloc_asprintf(state, "\\\\%s\\%s",
				      smbXcli_conn_remote_name(cli->conn),
				      share);
		if (tevent_req_nomem(unc, req)) {
			return tevent_req_post(req, ev);
		}

		subreq = smb2cli_tcon_send(state, ev, cli->conn, cli->timeout,
					   cli->smb2.session, cli->smb2.tcon,
					   0, /* flags */
					   unc);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_tree_connect_smb2_done,
					req);
		return req;
	}

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_LANMAN1) {
		subreq = cli_tcon_andx_send(state, ev, cli, share, dev,
					    pass, passlen);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_tree_connect_andx_done,
					req);
		return req;
	}

	subreq = cli_raw_tcon_send(state, ev, cli, share, pass, dev);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_tree_connect_raw_done, req);

	return req;
}