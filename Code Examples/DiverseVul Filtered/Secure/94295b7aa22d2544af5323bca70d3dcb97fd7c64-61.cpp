NTSTATUS cli_full_connection(struct cli_state **output_cli,
			     const char *my_name,
			     const char *dest_host,
			     const struct sockaddr_storage *dest_ss, int port,
			     const char *service, const char *service_type,
			     const char *user, const char *domain,
			     const char *password, int flags,
			     int signing_state)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_full_connection_send(
		ev, ev, my_name, dest_host, dest_ss, port, service,
		service_type, user, domain, password, flags, signing_state);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_full_connection_recv(req, output_cli);
 fail:
	TALLOC_FREE(ev);
	return status;
}