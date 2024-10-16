static void cli_session_setup_spnego_done_ntlmssp(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_session_setup_spnego_state *state = tevent_req_data(
		req, struct cli_session_setup_spnego_state);
	NTSTATUS status;

	status = cli_session_setup_gensec_recv(subreq);
	TALLOC_FREE(subreq);
	state->result = ADS_ERROR_NT(status);
	tevent_req_done(req);
}