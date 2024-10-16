NTSTATUS cli_raw_tcon(struct cli_state *cli,
		      const char *service, const char *pass, const char *dev,
		      uint16_t *max_xmit, uint16_t *tid)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_raw_tcon_send(ev, ev, cli, service, pass, dev);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_raw_tcon_recv(req, max_xmit, tid);
fail:
	TALLOC_FREE(ev);
	return status;
}