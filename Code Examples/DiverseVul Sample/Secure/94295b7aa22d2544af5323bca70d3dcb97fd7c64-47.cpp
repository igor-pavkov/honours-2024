NTSTATUS cli_ulogoff(struct cli_state *cli)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		status = smb2cli_logoff(cli->conn,
					cli->timeout,
					cli->smb2.session);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		smb2cli_session_set_id_and_flags(cli->smb2.session,
						 UINT64_MAX, 0);
		return NT_STATUS_OK;
	}

	if (smbXcli_conn_has_async_calls(cli->conn)) {
		return NT_STATUS_INVALID_PARAMETER;
	}
	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_ulogoff_send(ev, ev, cli);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_ulogoff_recv(req);
fail:
	TALLOC_FREE(ev);
	return status;
}