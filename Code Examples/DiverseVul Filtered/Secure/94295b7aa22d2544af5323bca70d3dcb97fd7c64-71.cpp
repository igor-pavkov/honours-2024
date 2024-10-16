static void cli_start_connection_connected(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_start_connection_state *state = tevent_req_data(
		req, struct cli_start_connection_state);
	NTSTATUS status;

	status = cli_connect_nb_recv(subreq, &state->cli);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	subreq = smbXcli_negprot_send(state, state->ev, state->cli->conn,
				      state->cli->timeout,
				      state->min_protocol,
				      state->max_protocol);
	if (tevent_req_nomem(subreq, req)) {
		return;
	}
	tevent_req_set_callback(subreq, cli_start_connection_done, req);
}