static void dns_tcp_call_loop(struct tevent_req *subreq)
{
	struct dns_tcp_connection *dns_conn = tevent_req_callback_data(subreq,
				      struct dns_tcp_connection);
	struct dns_server *dns = dns_conn->dns_socket->dns;
	struct dns_tcp_call *call;
	NTSTATUS status;

	call = talloc(dns_conn, struct dns_tcp_call);
	if (call == NULL) {
		dns_tcp_terminate_connection(dns_conn, "dns_tcp_call_loop: "
				"no memory for dns_tcp_call");
		return;
	}
	call->dns_conn = dns_conn;

	status = tstream_read_pdu_blob_recv(subreq,
					    call,
					    &call->in);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		const char *reason;

		reason = talloc_asprintf(call, "dns_tcp_call_loop: "
					 "tstream_read_pdu_blob_recv() - %s",
					 nt_errstr(status));
		if (!reason) {
			reason = nt_errstr(status);
		}

		dns_tcp_terminate_connection(dns_conn, reason);
		return;
	}

	DEBUG(10,("Received DNS TCP packet of length %lu from %s\n",
		 (long) call->in.length,
		 tsocket_address_string(dns_conn->conn->remote_address, call)));

	/* skip length header */
	call->in.data += 2;
	call->in.length -= 2;

	subreq = dns_process_send(call, dns->task->event_ctx, dns,
				  &call->in);
	if (subreq == NULL) {
		dns_tcp_terminate_connection(
			dns_conn, "dns_tcp_call_loop: dns_process_send "
			"failed\n");
		return;
	}
	tevent_req_set_callback(subreq, dns_tcp_call_process_done, call);

	/*
	 * The dns tcp pdu's has the length as 2 byte (initial_read_size),
	 * packet_full_request_u16 provides the pdu length then.
	 */
	subreq = tstream_read_pdu_blob_send(dns_conn,
					    dns_conn->conn->event.ctx,
					    dns_conn->tstream,
					    2, /* initial_read_size */
					    packet_full_request_u16,
					    dns_conn);
	if (subreq == NULL) {
		dns_tcp_terminate_connection(dns_conn, "dns_tcp_call_loop: "
				"no memory for tstream_read_pdu_blob_send");
		return;
	}
	tevent_req_set_callback(subreq, dns_tcp_call_loop, dns_conn);
}