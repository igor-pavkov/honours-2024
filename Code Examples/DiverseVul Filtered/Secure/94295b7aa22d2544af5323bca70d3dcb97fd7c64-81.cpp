static void cli_tcon_andx_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_tcon_andx_state *state = tevent_req_data(
		req, struct cli_tcon_andx_state);
	struct cli_state *cli = state->cli;
	uint8_t *in;
	uint8_t *inhdr;
	uint8_t wct;
	uint16_t *vwv;
	uint32_t num_bytes;
	uint8_t *bytes;
	NTSTATUS status;
	uint16_t optional_support = 0;

	status = cli_smb_recv(subreq, state, &in, 0, &wct, &vwv,
			      &num_bytes, &bytes);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}

	inhdr = in + NBT_HDR_SIZE;

	if (num_bytes) {
		if (clistr_pull_talloc(cli,
				(const char *)inhdr,
				SVAL(inhdr, HDR_FLG2),
				&cli->dev,
				bytes,
				num_bytes,
				STR_TERMINATE|STR_ASCII) == -1) {
			tevent_req_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}
	} else {
		cli->dev = talloc_strdup(cli, "");
		if (cli->dev == NULL) {
			tevent_req_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}
	}

	if ((smbXcli_conn_protocol(cli->conn) >= PROTOCOL_NT1) && (num_bytes == 3)) {
		/* almost certainly win95 - enable bug fixes */
		cli->win95 = True;
	}

	/*
	 * Make sure that we have the optional support 16-bit field. WCT > 2.
	 * Avoids issues when connecting to Win9x boxes sharing files
	 */

	if ((wct > 2) && (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_LANMAN2)) {
		optional_support = SVAL(vwv+2, 0);
	}

	if (optional_support & SMB_EXTENDED_SIGNATURES) {
		smb1cli_session_protect_session_key(cli->smb1.session);
	}

	smb1cli_tcon_set_values(state->cli->smb1.tcon,
				SVAL(inhdr, HDR_TID),
				optional_support,
				0, /* maximal_access */
				0, /* guest_maximal_access */
				NULL, /* service */
				NULL); /* fs_type */

	tevent_req_done(req);
}