static void cli_session_setup_spnego_done_krb(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_session_setup_spnego_state *state = tevent_req_data(
		req, struct cli_session_setup_spnego_state);
	NTSTATUS status;

	status = cli_session_setup_gensec_recv(subreq);
	TALLOC_FREE(subreq);
	state->result = ADS_ERROR_NT(status);

	if (ADS_ERR_OK(state->result) ||
	    !state->cli->fallback_after_kerberos) {
		tevent_req_done(req);
		return;
	}

	subreq = cli_session_setup_gensec_send(
		state, state->ev, state->cli,
		state->account, state->pass, state->user_domain,
		CRED_DONT_USE_KERBEROS,
		"cifs", state->target_hostname, NULL);
	if (tevent_req_nomem(subreq, req)) {
		return;
	}
	tevent_req_set_callback(subreq, cli_session_setup_spnego_done_ntlmssp,
				req);
}