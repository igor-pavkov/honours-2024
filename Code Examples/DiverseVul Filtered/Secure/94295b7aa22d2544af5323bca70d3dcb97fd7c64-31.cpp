static struct tevent_req *cli_connect_nb_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	const char *host, const struct sockaddr_storage *dest_ss,
	uint16_t port, int name_type, const char *myname,
	int signing_state, int flags)
{
	struct tevent_req *req, *subreq;
	struct cli_connect_nb_state *state;

	req = tevent_req_create(mem_ctx, &state, struct cli_connect_nb_state);
	if (req == NULL) {
		return NULL;
	}
	state->signing_state = signing_state;
	state->flags = flags;

	if (host != NULL) {
		char *p = strchr(host, '#');

		if (p != NULL) {
			name_type = strtol(p+1, NULL, 16);
			host = talloc_strndup(state, host, p - host);
			if (tevent_req_nomem(host, req)) {
				return tevent_req_post(req, ev);
			}
		}

		state->desthost = host;
	} else {
		state->desthost = print_canonical_sockaddr(state, dest_ss);
		if (tevent_req_nomem(state->desthost, req)) {
			return tevent_req_post(req, ev);
		}
	}

	subreq = cli_connect_sock_send(state, ev, host, name_type, dest_ss,
				       myname, port);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_connect_nb_done, req);
	return req;
}