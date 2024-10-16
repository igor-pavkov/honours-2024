static void cli_tree_connect_raw_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct cli_tree_connect_state *state = tevent_req_data(
		req, struct cli_tree_connect_state);
	NTSTATUS status;
	uint16_t max_xmit = 0;
	uint16_t tid = 0;

	status = cli_raw_tcon_recv(subreq, &max_xmit, &tid);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	smb1cli_tcon_set_values(state->cli->smb1.tcon,
				tid,
				0, /* optional_support */
				0, /* maximal_access */
				0, /* guest_maximal_access */
				NULL, /* service */
				NULL); /* fs_type */

	tevent_req_done(req);
}