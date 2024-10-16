static void cli_full_connection_sess_set_up(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_full_connection_state *state = tevent_req_data(
		req, struct cli_full_connection_state);
	NTSTATUS status;

	status = cli_session_setup_recv(subreq);
	TALLOC_FREE(subreq);

	if (!NT_STATUS_IS_OK(status) &&
	    (state->flags & CLI_FULL_CONNECTION_ANONYMOUS_FALLBACK)) {

		state->flags &= ~CLI_FULL_CONNECTION_ANONYMOUS_FALLBACK;

		subreq = cli_session_setup_send(
			state, state->ev, state->cli, "", "", 0, "", 0,
			state->domain);
		if (tevent_req_nomem(subreq, req)) {
			return;
		}
		tevent_req_set_callback(
			subreq, cli_full_connection_sess_set_up, req);
		return;
	}

	if (tevent_req_nterror(req, status)) {
		return;
	}

	if (state->service != NULL) {
		subreq = cli_tree_connect_send(
			state, state->ev, state->cli,
			state->service, state->service_type,
			state->password, state->pw_len);
		if (tevent_req_nomem(subreq, req)) {
			return;
		}
		tevent_req_set_callback(subreq, cli_full_connection_done, req);
		return;
	}

	tevent_req_done(req);
}