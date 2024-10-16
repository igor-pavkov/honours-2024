static void cli_tdis_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_tdis_state *state = tevent_req_data(
		req, struct cli_tdis_state);
	NTSTATUS status;

	status = cli_smb_recv(subreq, NULL, NULL, 0, NULL, NULL, NULL, NULL);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}
	cli_state_set_tid(state->cli, UINT16_MAX);
	tevent_req_done(req);
}