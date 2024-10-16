static void cli_full_connection_started(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_full_connection_state *state = tevent_req_data(
		req, struct cli_full_connection_state);
	NTSTATUS status;

	status = cli_start_connection_recv(subreq, &state->cli);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}
	subreq = cli_session_setup_send(
		state, state->ev, state->cli, state->user,
		state->password, state->pw_len, state->password, state->pw_len,
		state->domain);
	if (tevent_req_nomem(subreq, req)) {
		return;
	}
	tevent_req_set_callback(subreq, cli_full_connection_sess_set_up, req);
}