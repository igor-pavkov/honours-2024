static void dns_udp_call_loop(struct tevent_req *subreq)
{
	struct dns_udp_socket *sock = tevent_req_callback_data(subreq,
				      struct dns_udp_socket);
	struct dns_server *dns = sock->dns_socket->dns;
	struct dns_udp_call *call;
	uint8_t *buf;
	ssize_t len;
	int sys_errno;

	call = talloc(sock, struct dns_udp_call);
	if (call == NULL) {
		talloc_free(call);
		goto done;
	}
	call->sock = sock;

	len = tdgram_recvfrom_recv(subreq, &sys_errno,
				   call, &buf, &call->src);
	TALLOC_FREE(subreq);
	if (len == -1) {
		talloc_free(call);
		goto done;
	}

	call->in.data = buf;
	call->in.length = len;

	DEBUG(10,("Received DNS UDP packet of length %lu from %s\n",
		 (long)call->in.length,
		 tsocket_address_string(call->src, call)));

	subreq = dns_process_send(call, dns->task->event_ctx, dns,
				  &call->in);
	if (subreq == NULL) {
		TALLOC_FREE(call);
		goto done;
	}
	tevent_req_set_callback(subreq, dns_udp_call_process_done, call);

done:
	subreq = tdgram_recvfrom_send(sock,
				      sock->dns_socket->dns->task->event_ctx,
				      sock->dgram);
	if (subreq == NULL) {
		task_server_terminate(sock->dns_socket->dns->task,
				      "no memory for tdgram_recvfrom_send",
				      true);
		return;
	}
	tevent_req_set_callback(subreq, dns_udp_call_loop, sock);
}