static NTSTATUS cli_session_setup_gensec_recv(struct tevent_req *req)
{
	struct cli_session_setup_gensec_state *state =
		tevent_req_data(req,
		struct cli_session_setup_gensec_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		cli_state_set_uid(state->cli, UID_FIELD_INVALID);
		return status;
	}
	return NT_STATUS_OK;
}