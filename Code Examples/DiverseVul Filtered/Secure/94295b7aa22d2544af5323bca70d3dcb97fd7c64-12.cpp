NTSTATUS cli_start_connection(struct cli_state **output_cli, 
			      const char *my_name, 
			      const char *dest_host, 
			      const struct sockaddr_storage *dest_ss, int port,
			      int signing_state, int flags)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_start_connection_send(ev, ev, my_name, dest_host, dest_ss,
					port, signing_state, flags);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_start_connection_recv(req, output_cli);
fail:
	TALLOC_FREE(ev);
	return status;
}