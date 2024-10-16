static ADS_STATUS cli_session_setup_spnego_recv(struct tevent_req *req)
{
	struct cli_session_setup_spnego_state *state = tevent_req_data(
		req, struct cli_session_setup_spnego_state);

	return state->result;
}