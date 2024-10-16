NTSTATUS cli_connect_nb(const char *host, const struct sockaddr_storage *dest_ss,
			uint16_t port, int name_type, const char *myname,
			int signing_state, int flags, struct cli_state **pcli)
{
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;

	ev = samba_tevent_context_init(talloc_tos());
	if (ev == NULL) {
		goto fail;
	}
	req = cli_connect_nb_send(ev, ev, host, dest_ss, port, name_type,
				  myname, signing_state, flags);
	if (req == NULL) {
		goto fail;
	}
	if (!tevent_req_set_endtime(req, ev, timeval_current_ofs(20, 0))) {
		goto fail;
	}
	if (!tevent_req_poll_ntstatus(req, ev, &status)) {
		goto fail;
	}
	status = cli_connect_nb_recv(req, pcli);
fail:
	TALLOC_FREE(ev);
	return status;
}