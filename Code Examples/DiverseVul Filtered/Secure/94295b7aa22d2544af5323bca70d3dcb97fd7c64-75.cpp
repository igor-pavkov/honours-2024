static void cli_session_setup_gensec_local_next(struct tevent_req *req)
{
	struct cli_session_setup_gensec_state *state =
		tevent_req_data(req,
		struct cli_session_setup_gensec_state);
	struct tevent_req *subreq = NULL;

	if (state->local_ready) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	subreq = gensec_update_send(state, state->ev,
			state->auth_generic->gensec_security,
			state->blob_in);
	if (tevent_req_nomem(subreq, req)) {
		return;
	}
	tevent_req_set_callback(subreq, cli_session_setup_gensec_local_done, req);
}