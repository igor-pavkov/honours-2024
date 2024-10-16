static void dns_process_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct dns_process_state *state = tevent_req_data(
		req, struct dns_process_state);
	WERROR ret;

	ret = dns_server_process_query_recv(
		subreq, state,
		&state->out_packet.answers, &state->out_packet.ancount,
		&state->out_packet.nsrecs,  &state->out_packet.nscount,
		&state->out_packet.additional, &state->out_packet.arcount);
	TALLOC_FREE(subreq);

	if (!W_ERROR_IS_OK(ret)) {
		state->dns_err = werr_to_dns_err(ret);
	}
	tevent_req_done(req);
}