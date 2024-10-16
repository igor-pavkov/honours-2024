static void cli_session_setup_gensec_remote_next(struct tevent_req *req)
{
	struct cli_session_setup_gensec_state *state =
		tevent_req_data(req,
		struct cli_session_setup_gensec_state);
	struct tevent_req *subreq = NULL;

	if (state->remote_ready) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	subreq = cli_sesssetup_blob_send(state, state->ev,
					 state->cli, state->blob_out);
	if (tevent_req_nomem(subreq, req)) {
		return;
	}
	tevent_req_set_callback(subreq,
				cli_session_setup_gensec_remote_done,
				req);
}