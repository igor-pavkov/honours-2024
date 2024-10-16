static struct tevent_req *cli_connect_sock_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	const char *host, int name_type, const struct sockaddr_storage *pss,
	const char *myname, uint16_t port)
{
	struct tevent_req *req, *subreq;
	struct cli_connect_sock_state *state;
	const char *prog;
	struct sockaddr_storage *addrs;
	unsigned i, num_addrs;
	NTSTATUS status;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_connect_sock_state);
	if (req == NULL) {
		return NULL;
	}

	prog = getenv("LIBSMB_PROG");
	if (prog != NULL) {
		state->fd = sock_exec(prog);
		if (state->fd == -1) {
			status = map_nt_error_from_unix(errno);
			tevent_req_nterror(req, status);
		} else {
			state->port = 0;
			tevent_req_done(req);
		}
		return tevent_req_post(req, ev);
	}

	if ((pss == NULL) || is_zero_addr(pss)) {

		/*
		 * Here we cheat. resolve_name_list is not async at all. So
		 * this call will only be really async if the name lookup has
		 * been done externally.
		 */

		status = resolve_name_list(state, host, name_type,
					   &addrs, &num_addrs);
		if (!NT_STATUS_IS_OK(status)) {
			tevent_req_nterror(req, status);
			return tevent_req_post(req, ev);
		}
	} else {
		addrs = talloc_array(state, struct sockaddr_storage, 1);
		if (tevent_req_nomem(addrs, req)) {
			return tevent_req_post(req, ev);
		}
		addrs[0] = *pss;
		num_addrs = 1;
	}

	state->called_names = talloc_array(state, const char *, num_addrs);
	if (tevent_req_nomem(state->called_names, req)) {
		return tevent_req_post(req, ev);
	}
	state->called_types = talloc_array(state, int, num_addrs);
	if (tevent_req_nomem(state->called_types, req)) {
		return tevent_req_post(req, ev);
	}
	state->calling_names = talloc_array(state, const char *, num_addrs);
	if (tevent_req_nomem(state->calling_names, req)) {
		return tevent_req_post(req, ev);
	}
	for (i=0; i<num_addrs; i++) {
		state->called_names[i] = host;
		state->called_types[i] = name_type;
		state->calling_names[i] = myname;
	}

	subreq = smbsock_any_connect_send(
		state, ev, addrs, state->called_names, state->called_types,
		state->calling_names, NULL, num_addrs, port);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_connect_sock_done, req);
	return req;
}