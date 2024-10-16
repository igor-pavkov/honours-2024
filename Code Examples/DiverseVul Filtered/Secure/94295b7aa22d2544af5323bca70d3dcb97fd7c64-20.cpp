static void cli_connect_nb_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_connect_nb_state *state = tevent_req_data(
		req, struct cli_connect_nb_state);
	NTSTATUS status;
	int fd = 0;
	uint16_t port;

	status = cli_connect_sock_recv(subreq, &fd, &port);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	state->cli = cli_state_create(state, fd, state->desthost, NULL,
				      state->signing_state, state->flags);
	if (tevent_req_nomem(state->cli, req)) {
		close(fd);
		return;
	}
	tevent_req_done(req);
}