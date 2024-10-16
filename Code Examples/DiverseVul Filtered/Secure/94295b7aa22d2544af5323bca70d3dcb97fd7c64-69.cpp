NTSTATUS cli_tcon_andx(struct cli_state *cli, const char *share,
		       const char *dev, const char *pass, int passlen)
{
	TALLOC_CTX *frame = talloc_stackframe();
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	if (smbXcli_conn_has_async_calls(cli->conn)) {
		/*
		 * Can't use sync call while an async call is in flight
		 */
		status = NT_STATUS_INVALID_PARAMETER;
		goto fail;
	}

	ev = samba_tevent_context_init(frame);
	if (ev == NULL) {
		goto fail;
	}

	req = cli_tcon_andx_send(frame, ev, cli, share, dev, pass, passlen);
	if (req == NULL) {
		goto fail;
	}

	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}

	status = cli_tcon_andx_recv(req);
 fail:
	TALLOC_FREE(frame);
	return status;
}