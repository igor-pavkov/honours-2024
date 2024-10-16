static void cli_session_setup_gensec_local_done(struct tevent_req *subreq)
{
	struct tevent_req *req =
		tevent_req_callback_data(subreq,
		struct tevent_req);
	struct cli_session_setup_gensec_state *state =
		tevent_req_data(req,
		struct cli_session_setup_gensec_state);
	NTSTATUS status;

	status = gensec_update_recv(subreq, state, &state->blob_out);
	TALLOC_FREE(subreq);
	state->blob_in = data_blob_null;
	if (!NT_STATUS_IS_OK(status) &&
	    !NT_STATUS_EQUAL(status, NT_STATUS_MORE_PROCESSING_REQUIRED))
	{
		tevent_req_nterror(req, status);
		return;
	}

	if (NT_STATUS_IS_OK(status)) {
		state->local_ready = true;
	}

	if (state->local_ready && state->remote_ready) {
		cli_session_setup_gensec_ready(req);
		return;
	}

	cli_session_setup_gensec_remote_next(req);
}