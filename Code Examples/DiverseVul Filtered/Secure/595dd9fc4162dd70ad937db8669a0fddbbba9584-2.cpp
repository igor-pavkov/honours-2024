static void winbindd_lookupsids_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct winbindd_lookupsids_state *state = tevent_req_data(
		req, struct winbindd_lookupsids_state);
	NTSTATUS status;

	status = wb_lookupsids_recv(subreq, state, &state->domains,
				    &state->names);
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}
	tevent_req_done(req);
}