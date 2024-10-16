struct tevent_req *cli_session_setup_guest_send(TALLOC_CTX *mem_ctx,
						struct tevent_context *ev,
						struct cli_state *cli)
{
	struct tevent_req *req, *subreq;
	NTSTATUS status;

	req = cli_session_setup_guest_create(mem_ctx, ev, cli, &subreq);
	if (req == NULL) {
		return NULL;
	}

	status = smb1cli_req_chain_submit(&subreq, 1);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return tevent_req_post(req, ev);
	}
	return req;
}