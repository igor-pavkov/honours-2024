NTSTATUS cli_tree_connect(struct cli_state *cli, const char *share,
			  const char *dev, const char *pass, int passlen)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	if (smbXcli_conn_has_async_calls(cli->conn)) {
		return NT_STATUS_INVALID_PARAMETER;
	}
	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_tree_connect_send(ev, ev, cli, share, dev, pass, passlen);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_tree_connect_recv(req);
fail:
	TALLOC_FREE(ev);
	return status;
}