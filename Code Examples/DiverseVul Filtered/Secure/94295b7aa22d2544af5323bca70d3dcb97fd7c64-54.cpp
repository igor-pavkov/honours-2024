NTSTATUS cli_full_connection_recv(struct tevent_req *req,
				  struct cli_state **output_cli)
{
	struct cli_full_connection_state *state = tevent_req_data(
		req, struct cli_full_connection_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		return status;
	}
	*output_cli = state->cli;
	talloc_set_destructor(state, NULL);
	return NT_STATUS_OK;
}