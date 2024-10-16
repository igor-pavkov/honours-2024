struct tevent_req *cli_tcon_andx_send(TALLOC_CTX *mem_ctx,
				      struct tevent_context *ev,
				      struct cli_state *cli,
				      const char *share, const char *dev,
				      const char *pass, int passlen)
{
	struct tevent_req *req, *subreq;
	NTSTATUS status;

	req = cli_tcon_andx_create(mem_ctx, ev, cli, share, dev, pass, passlen,
				   &subreq);
	if (req == NULL) {
		return NULL;
	}
	if (subreq == NULL) {
		return req;
	}
	status = smb1cli_req_chain_submit(&subreq, 1);
	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return tevent_req_post(req, ev);
	}
	return req;
}