static void cli_connect_sock_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_connect_sock_state *state = tevent_req_data(
		req, struct cli_connect_sock_state);
	NTSTATUS status;

	status = smbsock_any_connect_recv(subreq, &state->fd, NULL,
					  &state->port);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}
	set_socket_options(state->fd, lp_socket_options());
	tevent_req_done(req);
}