static void dns_udp_call_process_done(struct tevent_req *subreq)
{
	struct dns_udp_call *call = tevent_req_callback_data(
		subreq, struct dns_udp_call);
	struct dns_udp_socket *sock = call->sock;
	struct dns_server *dns = sock->dns_socket->dns;
	WERROR err;

	err = dns_process_recv(subreq, call, &call->out);
	TALLOC_FREE(subreq);
	if (!W_ERROR_IS_OK(err)) {
		DEBUG(1, ("dns_process returned %s\n", win_errstr(err)));
		TALLOC_FREE(call);
		return;
	}

	subreq = tdgram_sendto_queue_send(call,
					  dns->task->event_ctx,
					  sock->dgram,
					  sock->send_queue,
					  call->out.data,
					  call->out.length,
					  call->src);
	if (subreq == NULL) {
		talloc_free(call);
		return;
	}
	tevent_req_set_callback(subreq, dns_udp_call_sendto_done, call);

}