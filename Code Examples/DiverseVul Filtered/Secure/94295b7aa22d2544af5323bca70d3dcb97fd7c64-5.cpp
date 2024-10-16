static NTSTATUS cli_connect_nb_recv(struct tevent_req *req,
				    struct cli_state **pcli)
{
	struct cli_connect_nb_state *state = tevent_req_data(
		req, struct cli_connect_nb_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		return status;
	}
	*pcli = talloc_move(NULL, &state->cli);
	return NT_STATUS_OK;
}