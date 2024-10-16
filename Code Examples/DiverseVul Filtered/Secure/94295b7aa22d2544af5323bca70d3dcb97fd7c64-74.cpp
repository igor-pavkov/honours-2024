struct tevent_req *cli_full_connection_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	const char *my_name, const char *dest_host,
	const struct sockaddr_storage *dest_ss, int port,
	const char *service, const char *service_type,
	const char *user, const char *domain,
	const char *password, int flags, int signing_state)
{
	struct tevent_req *req, *subreq;
	struct cli_full_connection_state *state;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_full_connection_state);
	if (req == NULL) {
		return NULL;
	}
	talloc_set_destructor(state, cli_full_connection_state_destructor);

	state->ev = ev;
	state->service = service;
	state->service_type = service_type;
	state->user = user;
	state->domain = domain;
	state->password = password;
	state->flags = flags;

	state->pw_len = state->password ? strlen(state->password)+1 : 0;
	if (state->password == NULL) {
		state->password = "";
	}

	subreq = cli_start_connection_send(
		state, ev, my_name, dest_host, dest_ss, port,
		signing_state, flags);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_full_connection_started, req);
	return req;
}