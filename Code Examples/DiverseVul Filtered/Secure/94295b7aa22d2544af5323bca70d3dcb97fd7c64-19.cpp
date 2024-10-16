static struct tevent_req *cli_ulogoff_send(TALLOC_CTX *mem_ctx,
				    struct tevent_context *ev,
				    struct cli_state *cli)
{
	struct tevent_req *req, *subreq;
	struct cli_ulogoff_state *state;

	req = tevent_req_create(mem_ctx, &state, struct cli_ulogoff_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;

	SCVAL(state->vwv+0, 0, 0xFF);
	SCVAL(state->vwv+1, 0, 0);
	SSVAL(state->vwv+2, 0, 0);

	subreq = cli_smb_send(state, ev, cli, SMBulogoffX, 0, 2, state->vwv,
			      0, NULL);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_ulogoff_done, req);
	return req;
}