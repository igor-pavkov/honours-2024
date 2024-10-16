static struct tevent_req *cli_tdis_send(TALLOC_CTX *mem_ctx,
				 struct tevent_context *ev,
				 struct cli_state *cli)
{
	struct tevent_req *req, *subreq;
	struct cli_tdis_state *state;

	req = tevent_req_create(mem_ctx, &state, struct cli_tdis_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;

	subreq = cli_smb_send(state, ev, cli, SMBtdis, 0, 0, NULL, 0, NULL);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_tdis_done, req);
	return req;
}