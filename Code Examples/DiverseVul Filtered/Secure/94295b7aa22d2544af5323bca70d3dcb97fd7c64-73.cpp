static void cli_session_setup_done_nt1(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	NTSTATUS status;

	status = cli_session_setup_nt1_recv(subreq);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(3, ("cli_session_setup: NT1 session setup "
			  "failed: %s\n", nt_errstr(status)));
		tevent_req_nterror(req, status);
		return;
	}
	tevent_req_done(req);
}