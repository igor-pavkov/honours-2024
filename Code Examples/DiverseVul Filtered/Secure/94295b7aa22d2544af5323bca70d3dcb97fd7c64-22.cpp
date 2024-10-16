static void cli_raw_tcon_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_raw_tcon_state *state = tevent_req_data(
		req, struct cli_raw_tcon_state);
	NTSTATUS status;

	status = cli_smb_recv(subreq, state, NULL, 2, NULL, &state->ret_vwv,
			      NULL, NULL);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}
	tevent_req_done(req);
}