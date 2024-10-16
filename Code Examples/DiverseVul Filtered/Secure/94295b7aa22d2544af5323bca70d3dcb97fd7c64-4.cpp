static void cli_session_setup_done_spnego(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	ADS_STATUS status;

	status = cli_session_setup_spnego_recv(subreq);
	TALLOC_FREE(subreq);
	if (!ADS_ERR_OK(status)) {
		DEBUG(3, ("SPNEGO login failed: %s\n", ads_errstr(status)));
		tevent_req_nterror(req, ads_ntstatus(status));
		return;
	}
	tevent_req_done(req);
}