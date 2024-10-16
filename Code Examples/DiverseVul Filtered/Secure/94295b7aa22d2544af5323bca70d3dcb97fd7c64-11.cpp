static struct tevent_req *cli_start_connection_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	const char *my_name, const char *dest_host,
	const struct sockaddr_storage *dest_ss, int port,
	int signing_state, int flags)
{
	struct tevent_req *req, *subreq;
	struct cli_start_connection_state *state;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_start_connection_state);
	if (req == NULL) {
		return NULL;
	}
	state->ev = ev;

	if (signing_state == SMB_SIGNING_IPC_DEFAULT) {
		state->min_protocol = lp_client_ipc_min_protocol();
		state->max_protocol = lp_client_ipc_max_protocol();
	} else {
		state->min_protocol = lp_client_min_protocol();
		state->max_protocol = lp_client_max_protocol();
	}

	subreq = cli_connect_nb_send(state, ev, dest_host, dest_ss, port,
				     0x20, my_name, signing_state, flags);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_start_connection_connected, req);
	return req;
}